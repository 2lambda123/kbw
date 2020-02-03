%skeleton "lalr1.cc" 
%require "3.5"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
    #include <string>
    #include <vector>
    #include <functional>

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
       BIT       "bit"
       IF        "if"
       CTRL      "ctrl"
       DUMP      "dump"
       MEASURE   "measure"
       OPEN_KET  "|"
       CLOSE_KET ">"
       EQ        "="
       ENDL      "end of line"
       EOF 0     "end of file"
       ;

%token <std::string>  GATE "gate"
%token <size_t> ID        

%nterm <size_t> Qbit
%nterm <std::vector<size_t>> QbitList 
%nterm <bool> BitList 

%printer { yyo << $$; } <*>;

%%
Start : Operation ENDL Start
      | EOF
      ;

Operation : "qubit" Qbit                               { drv.add_qubit($2);            }
          | "bit" ID                                   { drv.add_bit($2);              }
          | ID "=" "measure" Qbit                      { drv.measure($4, $1);          }
          | GATE QbitList                              { drv.gate($1, $2);             }
          | "ctrl" QbitList GATE QbitList              { drv.gate($3, $4, $2);         }
          | "if" BitList GATE QbitList                 { if ($2) drv.gate($3, $4);     }
          | "if" BitList "ctrl" QbitList GATE QbitList { if ($2) drv.gate($5, $6, $4); }
          | "dump"                                     { drv.dump();                   }        
          ;

QbitList : Qbit          { $$.push_back($1); }
         | Qbit QbitList { $$.push_back($1); $$.insert($$.end(), $2.begin(), $2.end()); }
         ;

BitList : ID         { $$ = drv.get_measure($1);        }
        | ID BitList { $$ = drv.get_measure($1) and $2; }
        ; 

Qbit : "|" ID ">"  { $$ = $2; }
        ;

%%

void yy::parser::error(const location_type& l, const std::string& m) {
  std::cerr << l << ": " << m << std::endl;
}

