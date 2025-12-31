#pragma once

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <map>

using namespace std;
using namespace llvm;

namespace AplCodegen {
class RValue {
  Value *resultPtr;
  Value *shapePtr;
  Value *shapeLength;

public:
  RValue(Value *resultPtr, Value *shapePtr, Value *shapeLength);
  Value *getResultPtr();
  Value *getShapePtr();
  Value *getShapeLength();
};

class LlvmCodegen {
  unique_ptr<LLVMContext> context;
  unique_ptr<Module> module;
  unique_ptr<IRBuilder<>> builder;
  DataLayout dataLayout;

  void initializeContextAndModule();

public:
  LlvmCodegen(llvm::DataLayout dataLayout);
  ~LlvmCodegen();

  pair<unique_ptr<LLVMContext>, unique_ptr<Module>>
  getAndReinitializeContextAndModule();

  pair<Value *, Value *> allocHeap(Value *size, Type* elemType);

  void print(string fmt, Value *val);
  void printResultCodegen(RValue returnExpr);

  void returnCodegen(Value *returnExpr);
  RValue literalCodegen(const vector<float> vec);

  // RValue negateCodegen(RValue arg);

  pair<BasicBlock *, Value *> addLoopStart(Value *loopIterInitialValue);
  void addLoopEnd(BasicBlock *loopBB, Value *nextIterVal,
                  Value *loopIterFinalValue);

  RValue addCodegen(RValue arg1, RValue arg2);
  // RValue subCodegen(RValue arg1, RValue arg2);
  // RValue mulCodegen(RValue arg1, RValue arg2);
  // RValue divCodegen(RValue arg1, RValue arg2);
};
} // namespace AplCodegen