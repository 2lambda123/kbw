%skeleton "lalr1.cc" 
%require "3.5"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
    #include <string>
    #include <vector>
    #include <complex>
    #include <tuple>

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
       ADJ       "adj"
       DUMP      "dump"
       DIRTY     "dirty"
       FREE      "free"
       MEASURE   "measure"
       OPEN_KET  "|"
       CLOSE_KET ">"
       EQ        "="
       OPEN_P    "("
       CLOSE_P   ")"
       ENDL      "end of line"
       EOF 0     "end of file"
       ;

%token <std::string> GATE 
%token <double>      DOUBLE 
%token <size_t>      ID        

%nterm <size_t> Qbit
%nterm <bool> BitList 
%nterm <std::vector<double>> DoubleList
%nterm <std::vector<size_t>> QbitList 
%nterm <std::tuple<std::string, std::vector<size_t>, std::vector<double>>> Gate 
%nterm <std::tuple<std::string, std::vector<size_t>, std::vector<double>, bool>> AdjGate 
%nterm <std::tuple<std::string, std::vector<size_t>, std::vector<double>, bool, std::vector<size_t>>> CtrlGate 

%printer { yyo << $$; } <*>;

%%
Start : Operation ENDL Start
      | EOF
      ;

Operation : "qubit" Qbit                 { drv.add_qubit($2);   }
          | "qubit" "dirty" Qbit         { drv.add_dirty($3);   }
          | "bit" ID                     { drv.add_bit($2);     }
          | "free" Qbit                  { drv.free($2);        }
          | "free" "dirty" Qbit          { drv.freedirty($3);   }
          | ID "=" "measure" Qbit        { drv.measure($4, $1); }
          | "if" BitList CtrlGate        { if ($2) drv.gate(std::get<0>($3), std::get<1>($3), std::get<2>($3), std::get<3>($3), std::get<4>($3)); }
          | CtrlGate                     { drv.gate(std::get<0>($1), std::get<1>($1), std::get<2>($1), std::get<3>($1), std::get<4>($1));         }
          | "dump"                       { drv.dump();          }        
          ;

Gate : GATE "(" DoubleList ")" QbitList  { $$ = std::make_tuple($1, $5, $3);                                  }
     | GATE QbitList                     { $$ = std::make_tuple($1, $2, std::vector<double>{}); }
     ;

AdjGate : "adj" Gate                     { $$ = std::make_tuple(std::get<0>($2), std::get<1>($2), std::get<2>($2), true);  }
        | Gate                           { $$ = std::make_tuple(std::get<0>($1), std::get<1>($1), std::get<2>($1), false); }
        ;

CtrlGate : "ctrl" QbitList AdjGate       { $$ = std::make_tuple(std::get<0>($3), std::get<1>($3), std::get<2>($3), std::get<3>($3), $2);                    }
         | AdjGate                       { $$ = std::make_tuple(std::get<0>($1), std::get<1>($1), std::get<2>($1), std::get<3>($1), std::vector<size_t>{}); }
         ;

DoubleList : DOUBLE                      { $$.push_back($1); }
           | DOUBLE DoubleList           { $$.push_back($1); $$.insert($$.end(), $2.begin(), $2.end()); }
           ;

QbitList : Qbit                          { $$.push_back($1); }
         | Qbit QbitList                 { $$.push_back($1); $$.insert($$.end(), $2.begin(), $2.end()); }
         ;

BitList : ID                             { $$ = drv.get_measure($1);        }
        | ID BitList                     { $$ = drv.get_measure($1) and $2; }
        ; 

Qbit : "|" ID ">"                        { $$ = $2; }
        ;

%%

void yy::parser::error(const location_type& l, const std::string& m) {
  std::cerr << l << ": " << m << std::endl;
}

