#pragma once

#include "parser.tab.hpp"
#include <string>
#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

class FooLexer : public yyFlexLexer {
public:
  int yylex(yy::parser::value_type *const yylval);
};