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

  Value *literalCodegen(const vector<float> vec);
  Value *addCodegen(Value *arg1, Value *arg2);
  Value *callCodegen();
  pair<unique_ptr<LLVMContext>, unique_ptr<Module>>
  getAndReinitializeContextAndModule();
  void returnCodegen(Value *returnExpr);
};
} // namespace AplCodegen