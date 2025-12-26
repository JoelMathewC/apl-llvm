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

// Configure the parser to accept the lexer and ast return ptr as an argument.
%parse-param {AplLexer &lexer}
// TODO: set this back to term
%parse-param {std::unique_ptr<AplAst::Node>& ast_ret_ptr}

// Using a union here prevents us from using smart pointers
// https://www.gnu.org/software/bison/manual/html_node/C_002b_002b-Unions.html
%define api.value.type variant

// TODO: set this back to term
%type <std::unique_ptr<AplAst::Node>> start prgm
%type <std::unique_ptr<AplAst::Node>> op_expr
%type <std::unique_ptr<AplAst::Literal>> array
%type <std::unique_ptr<AplAst::AssignStmt>> assign_stmt

%token <float> LITERAL
%token <std::string> VARIABLE
%token <char> INPUT_COMPLETED EXIT LEFT_ARROW DIAMOND MONADIC_OP DYADIC_OP

%right DIAMOND MONADIC_OP DYADIC_OP

%%
start: prgm INPUT_COMPLETED {ast_ret_ptr = std::move($1); YYACCEPT;}
    | INPUT_COMPLETED       {YYACCEPT;}
    | EXIT                  {exit(0);}

prgm: prgm DIAMOND prgm {}
    | op_expr           {$$ = std::move($1);} 
    | assign_stmt       {}

assign_stmt: VARIABLE LEFT_ARROW op_expr    {}

op_expr: '(' op_expr ')'        {$$ = std::move($2);}
    | MONADIC_OP op_expr          {}    
    | op_expr DYADIC_OP op_expr  {$$ = AplAst::Call::create($2, $1, $3);}    
    | array                     {$$ = std::move($1);}   
    | VARIABLE                  {$$ = AplAst::Variable::create($1);}

array: array LITERAL    {$$ = AplAst::Literal::create($1->getVal(),$2);}
    | LITERAL           {$$ = AplAst::Literal::create($1);}

%%

void yy::parser::error(const std::string &message)
{
    std::cerr << "Error: " << message << std::endl;
}