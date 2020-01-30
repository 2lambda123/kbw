%{
    #include <string>
    #include <cstdio>
    #include "parser.hpp"
    #include "../include/driver.hpp"
%}

%option noyywrap nounput noinput batch

GATE    [_\.a-zA-Z][_\.a-zA-Z_0-9]*
UINT    [0-9]+

%{
    #define YY_USER_ACTION  loc.columns(yyleng);
%}

%%

%{
    yy::location& loc = drv.location;
    loc.step ();
%}

[ ]+     loc.step();

\r\n?|\n loc.lines(yyleng); return yy::parser::make_ENDL(loc);

qubit    return yy::parser::make_QUBIT(loc);
measure  return yy::parser::make_MEASURE(loc);

\|       return yy::parser::make_OPEN_KET(loc);
>        return yy::parser::make_CLOSE_KET(loc);

{UINT}   return yy::parser::make_UINT(std::stoll(yytext), loc);
{GATE}   return yy::parser::make_GATE(yytext, loc);

.        throw yy::parser::syntax_error(loc, "invalid character: " + std::string(yytext));
         
<<EOF>>  return yy::parser::make_EOF(loc);

%%

int Driver::parse() {
    location.initialize();

    yyin = stdin;
    yy::parser parse(*this);

    return parse();
}
