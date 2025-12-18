%{
    #include "foolexer.hpp"
    #include "parser.tab.hpp"
    typedef yy::parser::token token;

    #undef  YY_DECL
    #define YY_DECL int FooLexer::yylex(yy::parser::value_type *const yylval)
%}

%option c++ noyywrap
%option yyclass="FooLexer"

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