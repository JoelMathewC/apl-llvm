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
  GlobalVariable *typeInfo;

  void initializeContextAndModule();

public:
  LlvmCodegen(llvm::DataLayout dataLayout);
  ~LlvmCodegen();

  pair<unique_ptr<LLVMContext>, unique_ptr<Module>>
  getAndReinitializeContextAndModule();

  // Wrappers over external functions
  pair<Value *, Value *> allocHeap(Value *size, Type *elemType);
  void print(string fmt, Value *val);
  void throwError();

  // Codegen Helper functions
  pair<BasicBlock *, Value *> addLoopStart(Value *loopIterInitialValue);
  void addLoopEnd(BasicBlock *loopBB, Value *nextIterVal,
                  Value *loopIterFinalValue);
  Value *sumArrShape(RValue arg);
  void verifyDyadicOperands(RValue arg1, RValue arg2);
  void printResultCodegen(RValue returnExpr);

  // Node-based codegen functions
  RValue literalCodegen(const vector<float> vec);

  // Monadic codegen functions
  RValue negateCodegen(RValue arg);
  RValue indexGenCodegen(RValue arg);

  // Dyadic codegen functions
  RValue addCodegen(RValue arg1, RValue arg2);
  RValue subCodegen(RValue arg1, RValue arg2);
  RValue mulCodegen(RValue arg1, RValue arg2);
  RValue divCodegen(RValue arg1, RValue arg2);
  RValue reshapeCodegen(RValue targetShape, RValue arg);
};
} // namespace AplCodegen