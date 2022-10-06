#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "BlockScope.h"
#include "SymbolTableItem.h"
class SymbolTable {
public:
    SymbolTable();
    void initSymbolTable();
    BlockScope& getBlockScope(BlockScopeHandle handle);
    BlockScope& getCurrentScope();
    BlockScope& getGlobalScope();
    BlockScopeHandle getCurrentScopeHandle();
    void pushScope(BlockScopeType type);
    void popScope();

    template <typename ItemType>
    std::pair<ItemType*, bool> insertItem(const std::string& name, typename ItemType::Data data);
    SymbolTableItem* findItem(const std::string& name);
    SymbolTableItem* findFunc(const std::string& name);
    void dumpTable(std::ostream& os);
    void clearSymbolTable();

private:
    std::vector<BlockScope> m_blockScopes;
    BlockScopeHandle m_currScopeHandle;
};

template <typename ItemType>
std::pair<ItemType*, bool> SymbolTable::insertItem(const std::string& name, typename ItemType::Data data) {
    if (std::is_base_of<SymbolTableItem, ItemType>::value) {
        auto pair = getCurrentScope().insertItem(std::unique_ptr<ItemType>(new ItemType(name, data)));
        return std::make_pair(dynamic_cast<ItemType*>(pair.first), pair.second);
    } else {
        return std::make_pair(nullptr, false);
    }
}

#endif