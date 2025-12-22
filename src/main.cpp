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
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"

using namespace std;

int main() {
  cout << "\033[32m=== APL 1.0 REPL ===\033[0m\n";
  cout << "Type \"quit()\" to exit this program\n";
  AplLexer lexer;
  std::unique_ptr<AplAst::Term> ast_ret_ptr;
  yy::parser parser(lexer, ast_ret_ptr);

  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  llvm::ExitOnError ExitOnErr;

  unique_ptr<AplCompiler::JITCompiler> jit =
      ExitOnErr(AplCompiler::JITCompiler::create());

  while (true) {
    auto codegenManager = make_unique<AplCodegen::LlvmCodegen>();

    cout << "\033[35m>>>\033[0m ";
    parser();
    auto llvmIr = ast_ret_ptr->codegen(codegenManager.get());

    // cout << *ast_ret_ptr << "\n"; // Printing out the AST
    // llvmIr->print(errs()); // Printing out the LLVM IR

    auto Sym = ExitOnErr(jit->lookup("__anon_expr"));
    auto *fp = Sym.toPtr<float (*)()>();
    cout << "Evaluated to: " << fp() << "\n";

    // jit compile
    auto rt = jit->getMainJITDylib().createResourceTracker();
    auto tsm =
        llvm::orc::ThreadSafeModule(std::move(codegenManager->getModule()),
                                    std::move(codegenManager->getContext()));
    ExitOnErr(rt->remove());
  }

  return 0;
}