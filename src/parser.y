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

%right DIAMOND PLUS MINUS TIMES STAR DIVIDE

%%

prgm: prgm DIAMOND prgm     {}
    | op_expr               {} 
    | assign_stmt           {}

assign_stmt: VARIABLE LEFT_ARROW op_expr {}

op_expr: '(' op_expr ')'        {}
    | PLUS op_expr              {}    
    | MINUS op_expr             {}
    | TIMES op_expr             {}
    | STAR op_expr              {}
    | DIVIDE op_expr            {}
    | op_expr PLUS op_expr      {}    
    | op_expr MINUS op_expr     {} 
    | op_expr TIMES op_expr     {}
    | op_expr STAR op_expr      {}
    | op_expr DIVIDE op_expr    {}
    | array                     {}   
    | VARIABLE                  {}

array: array LITERAL            {$$ = AplAst::Literal::create($1->getVal(),$2);}
    | LITERAL                   {$$ = AplAst::Literal::create($1);}

%%

int main() {
    printf("Welcome to the APL compiler!\n");
    printf("apl-compiler> ");
    yyparse();
    return 0;
}