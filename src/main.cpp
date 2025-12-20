#include "lexer/AplLexer.hpp"
#include "parser/parser.g.hpp"
#include <iostream>

int main() {
  printf("\033[32m=== APL Interpreter 1.0 ===\033[0m\n");
  printf("Type \"quit()\" to exit this program\n");
  AplLexer lexer;
  std::unique_ptr<AplAst::Term> ast_ret_ptr;
  yy::parser parser(lexer, ast_ret_ptr);

  while (true) {
    printf("\033[35m>>>\033[0m ");
    parser();
    std::cout << *ast_ret_ptr << "\n";
  }

  return 0;
}