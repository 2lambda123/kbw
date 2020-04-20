#include "antlr4-runtime.h"
#include "kqasmLexer.h"
#include "kqasmParser.h"
#include "kqasmBaseVisitor.h"
#include "../include/assembler.hpp"

#include <iostream>

int main(int argc, const char* argv[]) {
    std::ifstream file{argv[1]};
    antlr4::ANTLRInputStream input(file);
    kqasmLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    kqasmParser parser(&tokens); 

    auto* tree = parser.entry();

    Assembler assembler;
    Code code = assembler.visitEntry(tree);
    code.run();
}