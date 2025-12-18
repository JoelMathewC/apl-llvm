// https://stackoverflow.com/questions/76509844/how-do-you-interface-c-flex-with-c-bison
%require "3.8"
%language "c++"

%code requires {
    #include <iostream>
    #include <vector>
    #include "ast/ast.hpp"
    class FooLexer;
}

%{
    #include "foolexer.hpp"
%}

%parse-param {FooLexer &lexer} 

%code {
    #define yylex lexer.yylex
}

// Using a union here prevents us from using smart pointers
// https://www.gnu.org/software/bison/manual/html_node/C_002b_002b-Unions.html
%define api.value.type variant

%token <double> LITERAL
%token <std::string> VARIABLE
%token <char> LEFT_ARROW DIAMOND PLUS MINUS TIMES STAR DIVIDE INPUT_COMPLETED EXIT

%type <std::unique_ptr<AplAst::Term>> start prgm
%type <std::unique_ptr<AplAst::Node>> op_expr
%type <std::unique_ptr<AplAst::Literal>> array
%type <std::unique_ptr<AplAst::AssignStmt>> assign_stmt

%right DIAMOND PLUS MINUS TIMES STAR DIVIDE

%%
start: prgm INPUT_COMPLETED  {std::cout << *$1; $$ = std::move($1); YYACCEPT;}
    | INPUT_COMPLETED       {YYACCEPT;}
    | EXIT                  {exit(0);}

prgm: prgm DIAMOND prgm     {}
    | op_expr               {$$ = std::move($1);} 
    | assign_stmt           {$$ = std::move($1);}

assign_stmt: VARIABLE LEFT_ARROW op_expr {auto val = AplAst::Variable::create($1); $$ = AplAst::AssignStmt::create(val,$3);}

op_expr: '(' op_expr ')'        {$$ = std::move($2);}
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
    | array                     {$$ = std::move($1);}   
    | VARIABLE                  {$$ = AplAst::Variable::create($1);}

array: array LITERAL            {$$ = AplAst::Literal::create($1->getVal(),$2);}
    | LITERAL                   {$$ = AplAst::Literal::create($1);}

%%

void yy::parser::error(const std::string &message)
{
    std::cerr << "Error: " << message << std::endl;
}

int main() {
    printf("\033[32m=== APL Interpreter 1.0 ===\033[0m\n");
    printf("Type \"quit()\" to exit this program\n");
    FooLexer lexer;
    yy::parser parser(lexer);

    while(true) {
        printf("\033[35m>>>\033[0m ");
        auto ast = parser();
        printf("\n");
    }

    return 0;
}