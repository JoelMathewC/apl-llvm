#pragma once

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <map>

using namespace std;
using namespace llvm;

namespace Codegen {
class LlvmCodegen {
  unique_ptr<LLVMContext> context;
  unique_ptr<Module> module;
  unique_ptr<IRBuilder<>> builder;
  map<string, Value *> variableMap;

public:
  LlvmCodegen();
  ~LlvmCodegen();

  Value *literalCodegen(const vector<float> vec);
  Value *variableCodegen(string name);
  Value *callCodegen();
  Value *assignCodegen();
};
} // namespace Codegen