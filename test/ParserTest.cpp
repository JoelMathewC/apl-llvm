#include "../src/lexer/AplLexer.hpp"
#include "../src/parser/parser.g.hpp"
#include <gtest/gtest.h>

TEST(ParserTest, CanParseLiteral) {
  AplLexer lexer;
  std::unique_ptr<AplAst::Term> astRetPtr;
  yy::parser parser(lexer, astRetPtr);
}
