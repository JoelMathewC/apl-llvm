// Set language to c++ and version to be >= 3.2 to prevent
// materializing stack.hpp
%require "3.8"
%language "c++"

%code requires {
    #include <iostream>
    #include <vector>
    #include "../ast/ast.hpp"

    // Since the parser.g.hpp file will not include AplLexer
    // we define a declaration here for the time being.
    class AplLexer;
}

// Ensure that these includes are limited to the cpp to prevent
// cyclic dependency with AplLexer.
%code {
    #include "../lexer/AplLexer.hpp"
    #define yylex lexer.yylex
}

// Configure the parser to accept the lexer as an argument.
%parse-param {AplLexer &lexer}

// Using a union here prevents us from using smart pointers
// https://www.gnu.org/software/bison/manual/html_node/C_002b_002b-Unions.html
%define api.value.type variant

%type <std::unique_ptr<AplAst::Term>> start prgm
%type <std::unique_ptr<AplAst::Node>> op_expr
%type <std::unique_ptr<AplAst::Literal>> array
%type <std::unique_ptr<AplAst::AssignStmt>> assign_stmt

%token <double> LITERAL
%token <std::string> VARIABLE
%token <char> INPUT_COMPLETED EXIT LEFT_ARROW DIAMOND OPERATOR

%right DIAMOND OPERATOR

%%
start: prgm INPUT_COMPLETED {std::cout << *$1; $$ = std::move($1); YYACCEPT;}
    | INPUT_COMPLETED       {YYACCEPT;}
    | EXIT                  {exit(0);}

prgm: prgm DIAMOND prgm {}
    | op_expr           {$$ = std::move($1);} 
    | assign_stmt       {$$ = std::move($1);}

assign_stmt: VARIABLE LEFT_ARROW op_expr    {cout << $1; $$ = AplAst::AssignStmt::create($1,$3);}

op_expr: '(' op_expr ')'        {$$ = std::move($2);}
    | OPERATOR op_expr          {}    
    | op_expr OPERATOR op_expr  {$$ = AplAst::Call::create(AplAst::Operator::ADD, $1, $3);}    
    | array                     {$$ = std::move($1);}   
    | VARIABLE                  {$$ = AplAst::Variable::create($1);}

array: array LITERAL    {$$ = AplAst::Literal::create($1->getVal(),$2);}
    | LITERAL           {$$ = AplAst::Literal::create($1);}

%%

void yy::parser::error(const std::string &message)
{
    std::cerr << "Error: " << message << std::endl;
}