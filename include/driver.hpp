#pragma once
#include "../src/parser.hpp"

# define YY_DECL yy::parser::symbol_type yylex(Driver& drv)
YY_DECL;

class Driver {
    public:

    int parse();
    
    std::string file;
    yy::location location;
};