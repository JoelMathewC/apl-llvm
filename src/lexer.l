/*
    The yywrap() function specifies the behaviour of the lexer
    when it reaches the end of a file. By disabling yywrap()
    we expect it to terminate at the end of a file.
    Ref: https://stackoverflow.com/questions/1811125/undefined-reference-to-yywrap
*/
%option noyywrap

%{
    #include "parser.tab.hpp"
    int yyerror(const char *sp);
    int yyparse();
%}

%%

"←"     {return LEFT_ARROW;}

"+"		{return PLUS;}
"-"		{return MINUS;}
"x"		{return TIMES;}
"*"		{return STAR;}
"÷"		{return DIVIDE;}

[_a-zA-Z⍙∆]+ {return VARIABLE;}
[0-9]+(\.[0-9]+)? {return LITERAL;}

%%

int main() {
    yyparse();
}

int yyerror(const char *sp) {
    printf("%s", sp);
    exit(1);
}