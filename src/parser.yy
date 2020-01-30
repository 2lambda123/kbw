%skeleton "lalr1.cc" 
%require "3.5"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
    #include <string>

    class Driver;
}

%param { Driver& drv }

%locations

%define parse.error verbose

%code {
    #include "../include/driver.hpp"
}

%define api.token.prefix {TOK_}
%token QUBIT     "qubit"
       MEASURE   "measure"
       OPEN_KET  "|"
       CLOSE_KET ">"
       ENDL      "end of line"
       EOF 0     "end of file"
       ;

%token <std::string>  GATE
%token <size_t> UINT

%%
Start : Operation ENDL Start
      | EOF
      ;

Operation : "qubit" Qbit
          | "measure" Qbit
          | GATE List
          ;

List : Bit 
     | Bit List
     ;

Qbit : "|" UINT ">"

Bit : Qbit
    | UINT
    ;

%%

void yy::parser::error(const location_type& l, const std::string& m) {
  std::cerr << l << ": " << m << std::endl;
}

