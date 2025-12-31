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

void printResult(float *res, vector<unsigned long> shape) {
  string shapeStr = "";
  for (int i = 0; i < shape.size(); ++i) {
    if (i == 0)
      shapeStr.append(to_string(shape[i]));
    else
      shapeStr.append(to_string(shape[i]) + "x");
  }
  cout << "<" << shapeStr << "> [";

  unsigned long totalElem = 1;
  for (auto idx : shape)
    totalElem *= idx;

  for (int i = 0; i < totalElem; ++i) {
    if (i > 0)
      cout << " ";
    cout << *(res + i);
  }
  cout << "]\n";
}

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
    compiledFunc();
    // printResult(res, astRetPtr->getShape());
  }

  return 0;
}