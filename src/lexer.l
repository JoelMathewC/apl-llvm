/*
    The yywrap() function specifies the behaviour of the lexer
    when it reaches the end of a file. By disabling yywrap()
    we expect it to terminate at the end of a file.
    Ref: https://stackoverflow.com/questions/1811125/undefined-reference-to-yywrap
*/
%option noyywrap

%{
    #include "parser.tab.hpp"
    int yyerror(char *sp);
    int yyparse();
%}

%%

"←"     {return LEFT_ARROW;}

"+"		{return PLUS;}
"-"		{return MINUS;}
"x"		{return TIMES;}
"*"		{return STAR;}
"÷"		{return DIVIDE;}
"⍟"     {return LOG;}
"⌹"     {return DOMINO;}
"○"     {return CIRCLE;}
"!"     {return EXCLAMATION;}
"?"     {return QUESTION_MARK;}

"|"     {return STILE;}
"⌈"     {return UPSTILE;}
"⌊"     {return DOWNSTILE;}
"⊥"     {return UP_TACK;}
"⊤"     {return DOWN_TACK;}
"⊣"     {return LEFT_TACK;}
"⊢"     {return RIGHT_TACK;}

"="     {return EQUAL;}
"≠"     {return NOT_EQUAL;}
"≤"      {return LEQ;}
"<"      {return LE;}
"≥"      {return GEQ;}
">"      {return GE;}

"≡"     {return EQUAL_UNDERBAR;}
"≢"     {return EQUAL_UNDERBAR_SLASH;}

"↑" {return UP_ARROW;}
"↓" {return DOWN_ARROW;}
"⊂" {return LEFT_SHOE;}
"⊃" {return RIGHT_SHOE;}
"⊆" {return LEFT_SHOE_UNDERBAR;}
"⌷" {return SQUAD;}
"⍋" {return GRADE_UP;}
"⍒" {return GRADE_DOWN;}

"⍳" {return IOTA;}
"⍸" {return IOTA_UNDERBAR;}
"∊" {return EPSILON;}
"⍷" {return EPSILON_UNDERBAR;}
"∪" {return UP_SHOE;}
"∩" {return DOWN_SHOE;}
"~" {return TILDE;}

"/" {return SLASH;}
"\\" {return BACKSLASH;}
"⌿" {return SLASH_BAR;}
"⍀" {return BACKSLASH_BAR;}

"," {return COMMA;}
"⍪" {return COMMA_BAR;}
"⍴" {return RHO;}
"⌽" {return CIRCLE_STILE;}
"⊖" {return CIRCLE_BAR;}
"⍉" {return TRANSPOSE;}

"¨" {return DIAERESES;}
"⍨" {return TILDE_DIAERESES;}
"⍣" {return STAR_DIAERESES;}
"." {return DOT;}
"∘" {return JOT;}
"⍤" {return JOT_DIAERESES;}
"⍥" {return CIRCLE_DIAERESES;}
"@" {return AT;}

"⍞" {return QUOTE_QUAD;}
"⎕" {return QUAD;}
"⍠" {return QUAD_COLON;}
"⌸" {return QUAD_EQUAL;}
"⌺" {return QUAD_DIAMOND;}
"⌶" {return IBEAM;}
"⍎" {return HYDRANT;}
"⍕" {return THORN;}

"⋄" {return DIAMOND;}
"→" {return RIGHT_ARROW;}
"⍵" {return OMEGA;}
"⍺" {return ALPHA;}
"∇" {return DEL;}
"&" {return AMPERSAND;}

"¯" {return HIGH_MINUS;}
"⍬" {return ZILDE;}

[_a-zA-Z⍙∆]+ {return VARIABLE;}
[0-9]+(\.[0-9]+)? {return LITERAL;}
%%

int main() {
    yyparse();
}

int yyerror(char *sp) {
    printf("%s", sp);
    exit(1);
}