%{
    
    #include "AplLexer.hpp"
    #include "../parser/parser.g.hpp"
    #include <iostream>
    typedef yy::parser::token token;

    // Redefine the yylex() definition to include one argument.
    #undef  YY_DECL
    #define YY_DECL int AplLexer::yylex(yy::parser::value_type *const yylval)
%}

/*
 Set language to C++
 Assume that yywrap will always return 1
 Set the lexer class to be AplLexer, instead of the default
*/
%option c++ noyywrap yyclass="AplLexer"

/*
We don't diffrentiate between monadic and dyadic operators here since most
APL glyphs can serve as monadic and dyadic operators and the lexer cannot distinguish
without context so we differ the distinction to the parser.
*/
OPERATOR [+\-x*÷]

%%
[ \t]+     {}
(⍝.*)       {}

"\n"        {return token::INPUT_COMPLETED;}
"quit()"    {return token::EXIT;}
[()]	    {return *yytext;}

{OPERATOR}          {yylval->emplace<char>(*yytext); return token::OPERATOR;}
[0-9]+(\.[0-9]+)?   {yylval->emplace<float>(std::stod(yytext)); return token::LITERAL;}

.   {printf("unknown character: %c", *yytext); return *yytext;}
%%