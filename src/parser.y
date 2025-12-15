%{
    #include <stdio.h>
    #include <stdlib.h>
    int yylex();
    int yyerror(char *sp);
%}

%token VARIABLE LITERAL LEFT_ARROW PLUS MINUS TIMES STAR DIVIDE LOG DOMINO CIRCLE EXCLAMATION 
%token QUESTION_MARK STILE UPSTILE DOWNSTILE UP_TACK DOWN_TACK LEFT_TACK RIGHT_TACK EQUAL 
%token NOT_EQUAL LEQ LE GEQ GE EQUAL_UNDERBAR EQUAL_UNDERBAR_SLASH UP_ARROW DOWN_ARROW LEFT_SHOE 
%token RIGHT_SHOE LEFT_SHOE_UNDERBAR SQUAD GRADE_UP GRADE_DOWN IOTA IOTA_UNDERBAR EPSILON 
%token EPSILON_UNDERBAR UP_SHOE DOWN_SHOE TILDE SLASH BACKSLASH SLASH_BAR BACKSLASH_BAR COMMA 
%token COMMA_BAR RHO CIRCLE_STILE CIRCLE_BAR TRANSPOSE DIAERESES TILDE_DIAERESES STAR_DIAERESES DOT 
%token JOT JOT_DIAERESES CIRCLE_DIAERESES AT QUOTE_QUAD QUAD QUAD_COLON QUAD_EQUAL QUAD_DIAMOND 
%token IBEAM HYDRANT THORN DIAMOND RIGHT_ARROW OMEGA ALPHA DEL AMPERSAND HIGH_MINUS ZILDE

/*
Support pending
    | LOG 
    | DOMINO 
    | CIRCLE 
    | EXCLAMATION 
    | QUESTION_MARK 
    | STILE 
    | UPSTILE 
    | DOWNSTILE 
    | UP_TACK 
    | DOWN_TACK 
    | LEFT_TACK 
    | RIGHT_TACK 
    | EQUAL 
    | NOT_EQUAL 
    | LEQ 
    | LE 
    | GEQ 
    | GE 
    | EQUAL_UNDERBAR 
    | EQUAL_UNDERBAR_SLASH 
    | UP_ARROW 
    | DOWN_ARROW 
    | LEFT_SHOE 
    | RIGHT_SHOE 
    | LEFT_SHOE_UNDERBAR 
    | SQUAD 
    | GRADE_UP 
    | GRADE_DOWN 
    | IOTA 
    | IOTA_UNDERBAR 
    | EPSILON 
    | EPSILON_UNDERBAR 
    | UP_SHOE 
    | DOWN_SHOE 
    | TILDE 
    | SLASH 
    | BACKSLASH 
    | SLASH_BAR 
    | BACKSLASH_BAR 
    | COMMA 
    | COMMA_BAR 
    | RHO 
    | CIRCLE_STILE 
    | CIRCLE_BAR 
    | TRANSPOSE 
    | DIAERESES 
    | TILDE_DIAERESES 
    | STAR_DIAERESES 
    | DOT 
    | JOT 
    | JOT_DIAERESES 
    | CIRCLE_DIAERESES 
    | AT 
    | QUOTE_QUAD 
    | QUAD 
    | QUAD_COLON 
    | QUAD_EQUAL 
    | QUAD_DIAMOND 
    | IBEAM 
    | HYDRANT 
    | THORN 
    | DIAMOND 
    | RIGHT_ARROW 
    | OMEGA 
    | ALPHA 
    | DEL 
    | AMPERSAND 
    | HIGH_MINUS 
    | ZILDE
*/

%%

prgm: prgm '⋄' prgm     {}
    | '(' prgm ')'      {}  
    | op_stmt           {} 
    | assign_stmt       {}

assign_stmt: VARIABLE LEFT_ARROW op_stmt {}

op_stmt: '(' op_stmt ')'        {}
    | monadic_op op_stmt        {}    
    | op_stmt dyadic_op op_stmt {}    
    | array                     {}   
    | VARIABLE                  {}

monadic_op : PLUS   {}
    | MINUS         {}
    | TIMES         {}
    | STAR          {}
    | DIVIDE        {}
    

dyadic_op : PLUS    {}
    | MINUS         {}
    | TIMES         {}
    | STAR          {}
    | DIVIDE        {}

array: array LITERAL            {}
    | LITERAL                   {}

%%