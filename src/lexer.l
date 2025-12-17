%option noyywrap

%{
    #include "parser.tab.hpp"
    int yyerror(const char *sp);
%}

%%

"\n"    {return INPUT_COMPLETED;}
"quit()"    {return EXIT;}

[ \t]+
(⍝.*)

"←"     {return LEFT_ARROW;}
"+"		{return PLUS;}
"-"		{return MINUS;}
"x"		{return TIMES;}
"*"		{return STAR;}
"÷"		{return DIVIDE;}
"⋄"     {return DIAMOND;}

[_a-zA-Z⍙∆]+ {return VARIABLE;}
[0-9]+(\.[0-9]+)? {return LITERAL;}

%%

int yyerror(const char *sp) {
    printf("Error could not parse: %s", sp);
    exit(1);
}