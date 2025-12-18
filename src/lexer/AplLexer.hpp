#pragma once

#include "../parser/parser.g.hpp"
#include <string>
#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

// Custom Lexer class to interface with C++ Bison/Flex generated code.
// https://stackoverflow.com/questions/76509844/how-do-you-interface-c-flex-with-c-bison
// https://stackoverflow.com/questions/79685723/how-do-i-interface-flex-and-bison-using-c-with-variants
class AplLexer : public yyFlexLexer {
public:
  int yylex(yy::parser::value_type *const yylval);
};