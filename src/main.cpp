#include "lexer/AplLexer.hpp"
#include "parser/parser.g.hpp"
#include <iostream>

int main() {
  printf("\033[32m=== APL Interpreter 1.0 ===\033[0m\n");
  printf("Type \"quit()\" to exit this program\n");
  AplLexer lexer;
  yy::parser parser(lexer);

  while (true) {
    printf("\033[35m>>>\033[0m ");
    auto ast = parser();
  }

  return 0;
}