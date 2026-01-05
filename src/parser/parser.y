// Set language to c++ and version to be >= 3.2 to prevent
// materializing stack.hpp
%require "3.8"
%language "c++"

%code requires {
    #include <iostream>
    #include <vector>

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

%token <float> LITERAL
%token <char> INPUT_COMPLETED EXIT HIGH_MINUS

%right OPERATOR

%%
start: prgm INPUT_COMPLETED {YYACCEPT;}
    | INPUT_COMPLETED       {YYACCEPT;}
    | EXIT                  {exit(0);}

prgm: op_expr           {} 

op_expr: '(' op_expr ')'        {}
    | OPERATOR op_expr          {}    
    | op_expr OPERATOR op_expr  {}    
    | array                     {}

array: array LITERAL                {}
    | array HIGH_MINUS LITERAL      {}
    | HIGH_MINUS LITERAL            {}
    | LITERAL                       {}

%%

void yy::parser::error(const std::string &message)
{
    std::cerr << "Error: " << message << std::endl;
}