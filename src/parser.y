%code requires {
    #include <iostream>
    #include <vector>

    #include "ast/ast.hpp"

    int yylex();
    int yyerror(const char *sp);
}

%union {
    AplAst::Term* term;
    AplAst::Literal* literal;
    AplAst::AssignStmt* assign;
    AplAst::Call* call;
    const char* string;
    char single_char;
    double number;
}

%token <number> LITERAL
%token <string> VARIABLE
%token <single_char> LEFT_ARROW DIAMOND PLUS MINUS TIMES STAR DIVIDE INPUT_COMPLETED EXIT

%type <term> start prgm
%type <call> op_expr
%type <literal> array
%type <assign> assign_stmt

%right DIAMOND PLUS MINUS TIMES STAR DIVIDE

%%
start: prgm INPUT_COMPLETED  {$$ = $1; std::cout << *$1; YYACCEPT;}
    | INPUT_COMPLETED       {YYACCEPT;}
    | EXIT                  {exit(0);}

prgm: prgm DIAMOND prgm     {}
    | op_expr               {$$ = $1;} 
    | assign_stmt           {$$ = $1;}

assign_stmt: VARIABLE LEFT_ARROW op_expr {$$ = new AplAst::AssignStmt(new AplAst::Variable($1),$3);}

op_expr: '(' op_expr ')'        {$$ = $2;}
    | PLUS op_expr              {}    
    | MINUS op_expr             {}
    | TIMES op_expr             {}
    | STAR op_expr              {}
    | DIVIDE op_expr            {}
    | op_expr PLUS op_expr      {$$ = AplAst::Call::create(AplAst::Operator::ADD, $1, $3);}    
    | op_expr MINUS op_expr     {$$ = AplAst::Call::create(AplAst::Operator::SUB, $1, $3);} 
    | op_expr TIMES op_expr     {$$ = AplAst::Call::create(AplAst::Operator::MUL, $1, $3);}
    | op_expr STAR op_expr      {$$ = AplAst::Call::create(AplAst::Operator::EXP, $1, $3);}
    | op_expr DIVIDE op_expr    {$$ = AplAst::Call::create(AplAst::Operator::DIV, $1, $3);}
    | array                     {$$ = $1;}   
    | VARIABLE                  {$$ = new AplAst::Variable($1);}

array: array LITERAL            {$$ = AplAst::Literal::create($1->getVal(),$2);}
    | LITERAL                   {$$ = AplAst::Literal::create($1);}

%%

int main() {
    printf("\033[32m=== APL Interpreter 1.0 ===\033[0m\n");
    printf("Type \"quit()\" to exit this program\n");

    while(true) {
        printf("\033[35m>>>\033[0m ");
        AplAst::Term* ast = yyparse();
        printf("\n");
    }

    return 0;
}