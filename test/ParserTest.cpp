#include <gtest/gtest.h>
#include "../src/lexer/AplLexer.hpp"
#include "../src/parser/parser.g.hpp"

TEST(ParserTest, CanParseLiteral) {
  AplLexer lexer;
  yy::parser parser(lexer);
}
