%code requires {
    #include <cstdio>
    #include <cstdlib>
    #include <vector>

    #include "ast/ast.hpp"

    int yylex();
    int yyerror(const char *sp);
}

%union {
    AplAst::Literal* literal;
    const char* string;
    char single_char;
    double number;
}

%token <number> LITERAL
%token <string> VARIABLE
%token <single_char> LEFT_ARROW DIAMOND PLUS MINUS TIMES STAR DIVIDE
%type <literal> array

%right PLUS MINUS TIMES STAR DIVIDE

%%

prgm: prgm DIAMOND prgm     {}
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

array: array LITERAL            {$$ = AplAst::Literal::create($1->getVal(),$2);}
    | LITERAL                   {$$ = AplAst::Literal::create($1);}

%%