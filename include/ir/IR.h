#ifndef IR_H
#define IR_H
#include <vector>
#include <cassert>
#include <memory>
#include <set>
#include <map>

#include "IRTypeEnum.h"
#include <symbol/SymbolTableItem.h>

struct Use;
class Value {
public:
    explicit Value(IRType type) :
        m_type(type){};
    void addUse(Use* use) { m_uses.push_back(std::unique_ptr<Use>(use)); };
    void removeUse(Use* use) {
        for (auto it = m_uses.begin(); it != m_uses.end(); it++) {
            if (it->get() == use) {
                it = m_uses.erase(it);
                break;
            }
        }
    };

protected:
    IRType m_type;
    std::vector<std::unique_ptr<Use>> m_uses;
};

class Inst : public Value {
    friend class BasicBlock;

public:
    Inst(IRType type) :
        Value(type){};
    virtual std::vector<Value*> getOperands() { return {}; };
    virtual const std::string& toString() = 0;
};

class BasicBlock {
public:
    BasicBlock() = default;
    std::vector<BasicBlock*>& getPreds() { return m_pred; }
    std::array<BasicBlock*, 2> getSuccs();
    Inst* pushBackInst(Inst* inst) {
        m_insts.push_back(std::unique_ptr<Inst>(inst));
        return m_insts.back().get();
    };

private:
    std::vector<BasicBlock*> m_pred;
    bool m_vis{false};
    std::vector<std::unique_ptr<Inst>> m_insts;
};

class Function {
public:
    explicit Function(FuncItem* funcItem) :
        m_funcItem(funcItem) {}
    BasicBlock* pushBackBasicBlock(BasicBlock* basicBlock) {
        m_basicBlocks.push_back(std::unique_ptr<BasicBlock>(basicBlock));
        return m_basicBlocks.back().get();
    }
    void addCallee(Function* func) { m_callee.insert(func); }
    void addCaller(Function* func) { m_caller.insert(func); }

private:
    FuncItem* m_funcItem{nullptr};
    std::vector<std::unique_ptr<BasicBlock>> m_basicBlocks;
    std::set<Function*> m_callee;
    std::set<Function*> m_caller;
    bool m_isBuiltin{false};
};

class Module {
public:
    Function* addFunc(Function* func) {
        m_funcs.push_back(std::unique_ptr<Function>(func));
        return m_funcs.back().get();
    }

    SymbolTableItem* addGlobVar(SymbolTableItem* item) {
        m_globalVariables.push_back(item);
        return item;
    }

private:
    std::vector<std::unique_ptr<Function>> m_funcs;
    std::vector<SymbolTableItem*> m_globalVariables;
};

struct Use {
    Value* value{nullptr};
    Inst* user{nullptr};
    Use() = default;
    Use(Value* v, Inst* u) :
        value(v), user(u) {
        if (v) v->addUse(this);
    }
    Use(const Use& rhs) :
        value(rhs.value), user(rhs.user) {
        if (value) value->addUse(this);
    }
    Use& operator=(const Use& rhs) {
        if (this != &rhs) {
            assert(user == rhs.user);
            set(rhs.value);
        }
        return *this;
    }
    void set(Value* v) {
        if (value) value->removeUse(this);
        value = v;
        if (v) v->addUse(this);
    }
    ~Use() {
        if (value) value->removeUse(this);
    }
};

class BinaryInst : public Inst {
public:
    BinaryInst(IRType type, Value* lhs, Value* rhs) :
        Inst(type), m_lhs(lhs, this), m_rhs(rhs, this) {}

    bool rhsCanBeImm() {
        // Add, Sub, Rsb, Mul, Div, Mod, Lt, Le, Ge, Gt, Eq, Ne, And, Or
        return (m_type >= IRType::Add && m_type <= IRType::Rsb) || (m_type >= IRType::Lt && m_type <= IRType::Or);
    }

    constexpr static const char* LLVM_OPS[14] = {
        /* Add = */ "add",
        /* Sub = */ "sub",
        /* Rsb = */ nullptr,
        /* Mul = */ "mul",
        /* Div = */ "sdiv",
        /* Mod = */ "srem",
        /* Lt = */ "icmp slt",
        /* Le = */ "icmp sle",
        /* Ge = */ "icmp sge",
        /* Gt = */ "icmp sgt",
        /* Eq = */ "icmp eq",
        /* Ne = */ "icmp ne",
        /* And = */ "and",
        /* Or = */ "or",
    };

    constexpr static std::pair<IRType, IRType> swapableOperators[11] = {
        {IRType::Add, IRType::Add},
        {IRType::Sub, IRType::Rsb},
        {IRType::Mul, IRType::Mul},
        {IRType::Lt, IRType::Gt},
        {IRType::Le, IRType::Ge},
        {IRType::Gt, IRType::Lt},
        {IRType::Ge, IRType::Le},
        {IRType::Eq, IRType::Eq},
        {IRType::Ne, IRType::Ne},
        {IRType::And, IRType::And},
        {IRType::Or, IRType::Or},
    };

    bool swapOperand() {
        for (auto pair : swapableOperators) {
            auto& before = pair.first;
            auto& after = pair.second;
            if (m_type == before) {
                m_type = after;
                Value *l = m_lhs.value, *r = m_rhs.value;
                l->removeUse(&m_lhs);
                r->removeUse(&m_rhs);
                l->addUse(&m_rhs);
                r->addUse(&m_lhs);
                std::swap(m_lhs.value, m_rhs.value);
                return true;
            }
        }
        return false;
    }
    // operands
    // loop unroll pass里用到了lhs和rhs的摆放顺序，不要随便修改
private:
    Use m_lhs;
    Use m_rhs;
};

class GlobalVariable : public Value {
public:
    explicit GlobalVariable(SymbolTableItem* globalItem) :
        Value(IRType::Global), m_globalItem(globalItem) {}

private:
    SymbolTableItem* m_globalItem;
};

class ParamVariable : public Value {
public:
    explicit ParamVariable(SymbolTableItem* paramItem) :
        Value(IRType::Param), m_paramItem(paramItem) {}

private:
    SymbolTableItem* m_paramItem;
};

class ConstValue : public Value {
public:
    static ConstValue* get(int imm) {
        auto pair = POOL.insert({imm, nullptr});
        auto& inserted = pair.second;
        auto& it = pair.first;
        if (inserted) it->second = new ConstValue(imm);
        return it->second;
    }

private:
    const int m_imm;
    static std::map<int, ConstValue*> POOL;

    // use ConstValue::get instead
    explicit ConstValue(int imm) :
        Value(IRType::Const), m_imm(imm) {}
};

struct BranchInst : public Inst {
    friend class BasicBlock;

public:
    explicit BranchInst(Value* cond, BasicBlock* left, BasicBlock* right) :
        Inst(IRType::Branch), m_cond(cond, this), m_left(left), m_right(right) {}

private:
    Use m_cond;
    // true
    BasicBlock* m_left;
    // false
    BasicBlock* m_right;
};

class JumpInst : public Inst {
    friend class BasicBlock;

public:
    explicit JumpInst(BasicBlock* next) :
        Inst(IRType::Jump), m_next(next) {}

private:
    BasicBlock* m_next;
};

class ReturnInst : public Inst {
    friend class BasicBlock;

public:
    explicit ReturnInst(Value* ret) :
        Inst(IRType::Return), m_ret(ret, this) {}

private:
    Use m_ret;
};

struct AccessInst : Inst {
    friend class BasicBlock;

public:
    explicit AccessInst(IRType type, SymbolTableItem* lhs_sym, Value* arr, Value* index) :
        Inst(type), m_lhsSym(lhs_sym), m_arr(arr, this), m_index(index, this) {}

protected:
    SymbolTableItem* m_lhsSym;
    Use m_arr;
    Use m_index;
};

class GetElementPtrInst : public AccessInst {
public:
    explicit GetElementPtrInst(SymbolTableItem* lhsSym, Value* arr, Value* index, int multiplier) :
        AccessInst(IRType::GetElementPtr, lhsSym, arr, index), m_multiplier(multiplier) {}

private:
    int m_multiplier;
};

class LoadInst : public AccessInst {
public:
    explicit LoadInst(SymbolTableItem* lhsSym, Value* arr, Value* index) :
        AccessInst(IRType::Load, lhsSym, arr, index), m_memToken(nullptr, this) {}

private:
    Use m_memToken; // 由memdep pass计算
};

class StoreInst : public AccessInst {
public:
    explicit StoreInst(SymbolTableItem* lhsSym, Value* arr, Value* data, Value* index) :
        AccessInst(IRType::Store, lhsSym, arr, index), m_data(data, this) {}

private:
    Use m_data;
};

class CallInst : public Inst {
public:
    explicit CallInst(Function* func) :
        Inst(IRType::Call), m_func(func) {}

private:
    Function* m_func;
    std::vector<Use> m_args;
};

class AllocaInst : public Inst {
public:
    AllocaInst(SymbolTableItem* sym) :
        Inst(IRType::Alloca), m_sym(sym) {}

private:
    SymbolTableItem* m_sym;
};

struct CodeContext {
    Module module;
    Function* function;
    BasicBlock* basicBlock;
    std::vector<std::pair<BasicBlock*, BasicBlock*>> loop_stk; // <continue, break>
};

#endif