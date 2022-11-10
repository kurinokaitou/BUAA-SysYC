#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H
#include "Visitor.h"

class CodeGenerator {
public:
    CodeGenerator(std::shared_ptr<VNodeBase> astRoot);
    void generate();
    void dumpTable(std::filebuf& file);
    void dumpIr(std::filebuf& file, bool isTest);

private:
    std::unique_ptr<Visitor> m_visitor;
    SymbolTable m_table;
    CodeContext m_ctx;
};
#endif
