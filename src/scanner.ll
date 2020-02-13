%{
    #include <string>
    #include <cstdio>
    #include "parser.hpp"
    #include "../include/driver.hpp"
%}

%option noyywrap nounput noinput batch

GATE   [_a-z][_\.a-zA-Z_0-9]*
ORACLE \"[\x20-\x7e]+\" 
DOUBLE -?[0-9]+\.[0-9]+
ID     [0-9]+

%{
    #define YY_USER_ACTION  loc.columns(yyleng);
%}

%%

%{
    yy::location& loc = drv.location;
    loc.step();
%}

[ ]+     loc.step();

\r\n?|\n loc.lines(yyleng); return yy::parser::make_ENDL(loc);

qubit     return yy::parser::make_QUBIT(loc);
bit       return yy::parser::make_BIT(loc);
measure   return yy::parser::make_MEASURE(loc);
dump      return yy::parser::make_DUMP(loc);
if        return yy::parser::make_IF(loc);
ctrl      return yy::parser::make_CTRL(loc);
adj       return yy::parser::make_ADJ(loc);
dirty     return yy::parser::make_DIRTY(loc);
free      return yy::parser::make_FREE(loc);

\|        return yy::parser::make_OPEN_KET(loc);
>         return yy::parser::make_CLOSE_KET(loc);
=         return yy::parser::make_EQ(loc);
\(        return yy::parser::make_OPEN_P(loc);
\)        return yy::parser::make_CLOSE_P(loc);

{ID}      return yy::parser::make_ID(std::stoll(yytext), loc);
{GATE}    return yy::parser::make_GATE(yytext, loc);
{ORACLE}  return yy::parser::make_GATE(yytext, loc);
{DOUBLE}  return yy::parser::make_DOUBLE(std::stod(yytext), loc);

.         throw yy::parser::syntax_error(loc, "invalid character: " + std::string(yytext));
         
<<EOF>>   return yy::parser::make_EOF(loc);

%%

int Driver::parse() {
    location.initialize();

    yyin = stdin;
    //yyin = fopen("out.qasm", "r");
    yy::parser parse(*this);

    return parse();
}
