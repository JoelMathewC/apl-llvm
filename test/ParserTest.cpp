#include <gtest/gtest.h>
#include "../src/lexer/AplLexer.hpp"
#include "../src/parser/parser.g.hpp"

TEST(ParserTest, CanParseLiteral) {
  AplLexer lexer;
  std::unique_ptr<AplAst::Term> ast_ret_ptr;
  yy::parser parser(lexer, ast_ret_ptr);
}
