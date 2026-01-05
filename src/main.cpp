#include "ast/ast.hpp"
#include "lexer/AplLexer.hpp"
#include "parser/parser.g.hpp"
#include <iostream>

using namespace std;

int main() {
  cout << "\033[1;32m=== APL REPL v0.1 ===\033[0m\n";
  cout << "\033[3;37mType \"quit()\" to exit this program\033[0m\n";
  cout << "\033[3;37mAll outputs are of the form: <shape> [ array elements "
          "]\033[0m\n\n";
  AplLexer lexer;
  std::unique_ptr<AplAst::Node> astRetPtr;
  yy::parser parser(lexer, astRetPtr);

  while (true) {
    cout << "\033[35m>>>\033[0m ";
    try {
      parser();
    } catch (...) {
      cout << "Error constructing AST!\n";
    }

    if (astRetPtr != nullptr) {
      cout << *astRetPtr << "\n";
      astRetPtr = nullptr;
    }
  }

  return 0;
}