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
%token <single_char> LEFT_ARROW DIAMOND PLUS MINUS TIMES STAR DIVIDE INPUT_COMPLETED EXIT
%type <literal> array

%right DIAMOND PLUS MINUS TIMES STAR DIVIDE

%%
start: prgm                 {}
    | prgm INPUT_COMPLETED  {YYACCEPT;}
    | INPUT_COMPLETED       {YYACCEPT;}
    | EXIT                  {exit(0);}

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
    printf("\033[32m=== APL Interpreter 1.0 ===\033[0m\n");
    printf("Type \"quit()\" to exit this program\n");

    while(true) {
        printf("\033[35m>>>\033[0m ");
        yyparse();
    }

    return 0;
}