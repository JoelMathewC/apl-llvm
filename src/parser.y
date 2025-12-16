%{
    #include <cstdio>
    #include <cstdlib>

    int yylex();
    int yyerror(const char *sp);
%}

%token VARIABLE LITERAL LEFT_ARROW PLUS MINUS TIMES STAR DIVIDE 

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