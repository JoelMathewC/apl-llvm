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
  cout << "\033[32m=== APL 1.0 REPL ===\033[0m\n";
  cout << "Type \"quit()\" to exit this program\n";
  AplLexer lexer;
  std::unique_ptr<AplAst::Node> ast_ret_ptr;
  yy::parser parser(lexer, ast_ret_ptr);

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
    auto llvmIr = ast_ret_ptr->codegen(codegenManager.get(), true);
    auto [context, module] =
        codegenManager->getAndReinitializeContextAndModule();

    // module->print(errs(), nullptr);
    jit->compileAndExecute(std::move(context), std::move(module),
                           ast_ret_ptr->getShape());
  }

  return 0;
}