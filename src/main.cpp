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
  yy::parser parser(lexer);

  while (true) {
    cout << "\033[35m>>>\033[0m ";
    parser();
  }

  return 0;
}