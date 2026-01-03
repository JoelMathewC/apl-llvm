#include "codegen/codegen.hpp"
#include "compiler/compiler.hpp"
#include "lexer/AplLexer.hpp"
#include "parser/parser.g.hpp"
#include <iostream>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

using namespace std;

int main() {
  cout << "\033[32m=== APL REPL v0.1 ===\033[0m\n";
  cout << "Type \"quit()\" to exit this program\n";
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
    auto llvmIr = astRetPtr->codegen(codegenManager.get());
    auto compiledFunc = jit->compile(codegenManager.get(), llvmIr);

    try {
      compiledFunc();
    } catch (...) {
      cout << "Syntax Error!\n";
    }
  }

  return 0;
}