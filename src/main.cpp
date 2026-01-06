#include "codegen/codegen.hpp"
#include "compiler/compiler.hpp"
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

  unique_ptr<AplCompiler::JITCompiler> jit = AplCompiler::JITCompiler::create();
  if (jit == nullptr) {
    cout << "Could not initialize JIT. Exiting ...";
    exit(0);
  }

  auto codegenManager =
      make_unique<AplCodegen::LlvmCodegen>(jit->getDataLayout());

  while (true) {
    cout << "\033[35m>>>\033[0m ";
    parser();

    if (astRetPtr != nullptr) {
      auto llvmIr = astRetPtr->codegen(codegenManager.get());
      auto compiledFunc = jit->compile(codegenManager.get(), llvmIr);

      try {
        compiledFunc();
      } catch (...) {
        cout << "Error!\n";
      }
      astRetPtr = nullptr;
    }
  }

  return 0;
}