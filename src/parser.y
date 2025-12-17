%code requires {
    #include <iostream>
    #include <vector>

    #include "ast/ast.hpp"

    int yylex();
    int yyerror(const char *sp);
}

%union {
    AplAst::Node* node;
    AplAst::Literal* literal;
    const char* string;
    char single_char;
    double number;
}

%token <number> LITERAL
%token <string> VARIABLE
%token <single_char> LEFT_ARROW DIAMOND PLUS MINUS TIMES STAR DIVIDE INPUT_COMPLETED EXIT

%type <literal> prgm op_expr
%type <literal> array

%right DIAMOND PLUS MINUS TIMES STAR DIVIDE

%%
start: prgm INPUT_COMPLETED  {std::cout << *$1; YYACCEPT;}
    | INPUT_COMPLETED       {YYACCEPT;}
    | EXIT                  {exit(0);}

prgm: prgm DIAMOND prgm     {}
    | op_expr               {$$ = $1;} 
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
    | array                     {$$ = $1;}   
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
        printf("\n");
    }

    return 0;
}