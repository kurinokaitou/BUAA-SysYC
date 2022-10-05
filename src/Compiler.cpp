#include <Compiler.h>

static std::string s_sourcePath = "intermediate/test.txt";
static std::string s_dumpTokenPath = "intermediate/token.txt";
static std::string s_dumpASTPath = "intermediate/ast.txt";
static std::string s_dumpTablePath = "intermediate/table.txt";
static std::string s_dumpErrorPath = "intermediate/error.txt";
static bool s_dumpToken = false;
static bool s_dumpAST = false;
static bool s_dumpTable = false;
static bool s_dumpError = false;

static bool takeArg(char* arg) {
    static const std::set<std::string> x{"-o", "--dump-token", "--dump-ast", "--dump-table"};
    return x.count(std::string(arg));
}

static void usage(int status) {
    std::cerr << "usage: sysyc [--dump-token <path>] [--dump-ast <path>] [ -o <path> ] <path>\n";
    std::cerr << "usage: sysyc [--test]\n";
    exit(status);
}

void parseArgs(int argc, char** argv) {
    // TODO: 参数加入配置文件
    for (int i = 0; i < argc; i++) {
        if (takeArg(argv[i])) {
            if (!argv[++i])
                usage(1);
        }
    }
    for (int i = 1; i < argc; i++) {
        if (argv[i] == std::string("--dump-token")) {
            s_dumpTokenPath = argv[++i];
            s_dumpToken = true;
            continue;
        }
        if (argv[i] == std::string("--dump-ast")) {
            s_dumpASTPath = argv[++i];
            s_dumpAST = true;
            continue;
        }
        if (argv[i] == std::string("--dump-table")) {
            s_dumpTablePath = argv[++i];
            s_dumpTable = true;
            continue;
        }
        if (argv[i] == std::string("--dump-error")) {
            s_dumpErrorPath = argv[++i];
            s_dumpError = true;
            continue;
        }
        if (argv[i] == std::string("--test")) {
            s_dumpToken = true;
            s_dumpAST = true;
            s_dumpTable = true;
            s_dumpError = true;
            break;
        }
        s_sourcePath = std::string(argv[i]);
    }
}

bool Compiler::firstPass(std::filebuf& file) {
    try {
        m_tokenizer = std::unique_ptr<Tokenizer>(new Tokenizer(file));
        m_tokenList = m_tokenizer->tokenize();
        m_parser = std::unique_ptr<Parser>(new Parser(m_tokenList));
        m_parser->parse();
        m_generator = std::unique_ptr<CodeGenerator>(new CodeGenerator(m_parser->getASTRoot()));
        m_generator->generate();

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

void Compiler::dumpToken(std::filebuf& file) {
    if (!file.open(s_dumpTokenPath, std::ios::out)) {
        throw std::runtime_error("Fail to open the dump token file!");
    }
    std::ostream tos(&file);
    for (auto& token : m_tokenList) {
        tos << token;
    }
}

void Compiler::dumpAST(std::filebuf& file) {
    if (!file.open(s_dumpASTPath, std::ios::out)) {
        throw std::runtime_error("Fail to open the dump ast file!");
    }
    m_parser->traversalAST(file);
}

void Compiler::dumpTable(std::filebuf& file) {
    if (!file.open(s_dumpTablePath, std::ios::out)) {
        throw std::runtime_error("Fail to open the dump table file!");
    }
    m_generator->dumpTable(file);
}

void Compiler::dumpError(std::filebuf& file) {
    if (!file.open(s_dumpErrorPath, std::ios::out)) {
        throw std::runtime_error("Fail to open the dump error file!");
    }
    std::ostream os(&file);
    for (auto& log : s_errorDump) {
        os << log;
    }
}

int main(int argc, char** argv) {
    Compiler compiler;
    std::filebuf token;
    std::filebuf ast;
    std::filebuf table;
    std::filebuf error;
    std::filebuf in;

    parseArgs(argc, argv);
    if (!in.open(s_sourcePath, std::ios::in)) {
        throw std::runtime_error("Fail to open the source file!");
    }
    if (compiler.firstPass(in)) {
        if (s_dumpToken) {
            compiler.dumpToken(token);
        }
        if (s_dumpAST) {
            compiler.dumpAST(ast);
        }
        if (s_dumpTable) {
            compiler.dumpTable(table);
        }
        if (s_dumpError) {
            compiler.dumpError(error);
        }
    }
}
