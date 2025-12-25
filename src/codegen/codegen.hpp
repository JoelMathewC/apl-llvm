#pragma once

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <map>

using namespace std;
using namespace llvm;

namespace AplCodegen {
class LlvmCodegen {
  unique_ptr<LLVMContext> context;
  unique_ptr<Module> module;
  unique_ptr<IRBuilder<>> builder;
  DataLayout dataLayout;
  map<string, Value *> variableMap;

  void initializeContextAndModule();

public:
  LlvmCodegen(llvm::DataLayout dataLayout);
  ~LlvmCodegen();

  Value *literalCodegen(const float *vecPtr, int size);
  Value *variableCodegen(string name);
  Value *callCodegen();
  Value *assignCodegen();
  pair<unique_ptr<LLVMContext>, unique_ptr<Module>>
  getAndReinitializeContextAndModule();
  Function *wrapInAnonymousFunction(Value *exprIR);
};
} // namespace AplCodegen