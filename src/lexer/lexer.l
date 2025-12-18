%{
    #include "AplLexer.hpp"
    #include "../parser/parser.g.hpp"
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

%%

"\n"    {return token::INPUT_COMPLETED;}
"quit()"    {return token::EXIT;}

[ \t]+
(⍝.*)

"←"     {return token::LEFT_ARROW;}
"+"		{return token::PLUS;}
"-"		{return token::MINUS;}
"x"		{return token::TIMES;}
"*"		{return token::STAR;}
"÷"		{return token::DIVIDE;}
"⋄"     {return token::DIAMOND;}

[_a-zA-Z⍙∆]+ {return token::VARIABLE;}
[0-9]+(\.[0-9]+)? {return token::LITERAL;}

%%