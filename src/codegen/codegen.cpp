#include "codegen.hpp"
#include "../constants.hpp"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <iostream>
#include <map>

using namespace std;
using namespace llvm;

namespace AplCodegen {
Value *RValue::getResultPtr() { return this->resultPtr; }
Value *RValue::getShapePtr() { return this->shapePtr; }
Value *RValue::getShapeLength() { return this->shapeLength; }
RValue::RValue(Value *resultPtr, Value *shapePtr, Value *shapeLength)
    : resultPtr(resultPtr), shapePtr(shapePtr), shapeLength(shapeLength) {}

LlvmCodegen::LlvmCodegen(llvm::DataLayout dataLayout) {
  this->dataLayout = dataLayout;
  this->initializeContextAndModule();
}

LlvmCodegen::~LlvmCodegen() = default;

void LlvmCodegen::initializeContextAndModule() {
  this->context = make_unique<LLVMContext>();
  this->module = make_unique<Module>(Constants::moduleName, *this->context);
  this->module->setDataLayout(this->dataLayout);
  this->builder = make_unique<IRBuilder<>>(*this->context);

  // Define a function and set the builders insertion point to be a basic
  // block in the function.
  FunctionType *FT = FunctionType::get(this->builder->getPtrTy(),
                                       std::vector<Type *>(), false);
  Function *F =
      Function::Create(FT, Function::ExternalLinkage,
                       Constants::anonymousExprName, this->module.get());
  BasicBlock *BB = BasicBlock::Create(*this->context, "", F);
  this->builder->SetInsertPoint(BB);
}

pair<unique_ptr<LLVMContext>, unique_ptr<Module>>
LlvmCodegen::getAndReinitializeContextAndModule() {
  auto prev_context = std::move(this->context);
  auto prev_module = std::move(this->module);

  this->initializeContextAndModule();
  return make_pair(std::move(prev_context), std::move(prev_module));
}

pair<Value *, Value *> LlvmCodegen::allocHeap(Value *size, Type *elemType) {
  // Add the malloc function (if it doesnt exist).
  FunctionCallee mallocFunc = this->module->getOrInsertFunction(
      Constants::heapAllocFuncName, this->builder->getPtrTy(),
      Type::getInt32Ty(*this->context));

  uint64_t elementSize =
      this->module->getDataLayout().getTypeAllocSize(elemType);

  Value *totalSize = builder->CreateMul(
      ConstantInt::get(Type::getInt32Ty(*this->context), elementSize), size);

  return make_pair(this->builder->CreateCall(mallocFunc, totalSize), totalSize);
}

RValue LlvmCodegen::literalCodegen(const vector<float> vec) {
  // Create a global variable with the literal constant value.
  ArrayType *arrTy =
      ArrayType::get(Type::getFloatTy(*this->context), vec.size());
  Constant *init =
      ConstantDataArray::get(*this->context, ArrayRef(vec.data(), vec.size()));
  llvm::GlobalVariable *sourceGlobal = new llvm::GlobalVariable(
      *this->module, arrTy, true, llvm::GlobalValue::InternalLinkage, init);

  // Get the size that needs to be reserved and reserve it.
  auto [rawPtr, totalSize] =
      allocHeap(ConstantInt::get(Type::getInt32Ty(*this->context), vec.size()),
                this->builder->getFloatTy());

  // Initiate a memcpy of the global constant into the reserved memory location.
  this->builder->CreateMemCpy(rawPtr, MaybeAlign(0), sourceGlobal,
                              MaybeAlign(0), totalSize);

  // Get the size that needs to be reserved and reserve it.
  auto [shapeRawPtr, shapeSize] =
      allocHeap(ConstantInt::get(Type::getInt32Ty(*this->context), 1),
                this->builder->getInt32Ty());
  builder->CreateStore(builder->getInt32(vec.size()), shapeRawPtr);

  return RValue(rawPtr, shapeRawPtr,
                ConstantInt::get(Type::getInt32Ty(*this->context), 1));
}

pair<BasicBlock *, Value *>
LlvmCodegen::addLoopStart(Value *loopIterInitialValue) {
  // Allocate space to store the value of the iterator
  AllocaInst *alloca =
      this->builder->CreateAlloca(Type::getInt32Ty(*this->context), nullptr);
  builder->CreateStore(loopIterInitialValue, alloca);

  // Unconditional entry to loop block with builder set to insert instructions
  // there
  Function *func = this->builder->GetInsertBlock()->getParent();
  BasicBlock *LoopBB = BasicBlock::Create(*this->context, "", func);
  this->builder->CreateBr(LoopBB);
  this->builder->SetInsertPoint(LoopBB);
  return make_pair(LoopBB, alloca);
}

void LlvmCodegen::addLoopEnd(BasicBlock *loopBB, Value *nextIterVal,
                             Value *loopIterFinalValue) {
  Value *endCond = builder->CreateICmpULT(nextIterVal, loopIterFinalValue);

  // Create a block for execution after the loop and test if the end condition
  // is satisfied.
  Function *func = this->builder->GetInsertBlock()->getParent();
  BasicBlock *AfterLoopBB = BasicBlock::Create(*this->context, "", func);
  this->builder->CreateCondBr(endCond, loopBB, AfterLoopBB);
  this->builder->SetInsertPoint(AfterLoopBB);
}

RValue LlvmCodegen::addCodegen(RValue arg1, RValue arg2) {
  // TODO: test whether arg1.getShapeLength() == arg2.getShapeLength()
  // TODO: test elementwise comparison of shape idxs for both args

  // START SHAPE LOOP
  // At this point we have verified that the two arguments are of the same shape
  // We want to get a sum of all the elements that we need to operate on

  AllocaInst *sumAlloca =
      this->builder->CreateAlloca(Type::getInt32Ty(*this->context), nullptr);
  builder->CreateStore(this->builder->getInt32(1), sumAlloca);
  auto [shapeLoopBB, shapeIterAlloca] =
      this->addLoopStart(this->builder->getInt32(0));
  Value *shapeIterVal = this->builder->CreateLoad(
      Type::getInt32Ty(*this->context), shapeIterAlloca);

  Value *shapePtr = this->builder->CreateGEP(
      Type::getInt32Ty(*this->context), arg1.getShapePtr(), {shapeIterVal});
  Value *shapeVal =
      this->builder->CreateLoad(Type::getInt32Ty(*this->context), shapePtr);
  Value *oldSumVal =
      this->builder->CreateLoad(Type::getInt32Ty(*this->context), sumAlloca);
  Value *newSumVal = this->builder->CreateMul(oldSumVal, shapeVal);
  builder->CreateStore(newSumVal, sumAlloca);

  Value *nextShapeIterVal =
      this->builder->CreateAdd(shapeIterVal, builder->getInt32(1));
  this->builder->CreateStore(nextShapeIterVal, shapeIterAlloca);
  this->addLoopEnd(shapeLoopBB, nextShapeIterVal, arg1.getShapeLength());
  //   print("sumVal: %d", sumVal);
  // END SHAPE LOOP

  // START PROCESS LOOP
  Value *sumVal =
      this->builder->CreateLoad(Type::getInt32Ty(*this->context), sumAlloca);
  auto [resultBasePtr, resultSize] =
      allocHeap(sumVal, this->builder->getFloatTy());

  auto [processLoopBB, processIterAlloca] =
      this->addLoopStart(this->builder->getInt32(0));
  Value *iterVal = this->builder->CreateLoad(Type::getInt32Ty(*this->context),
                                             processIterAlloca);

  Value *arg1Ptr = this->builder->CreateGEP(Type::getFloatTy(*this->context),
                                            arg1.getResultPtr(), {iterVal});
  Value *arg2Ptr = this->builder->CreateGEP(Type::getFloatTy(*this->context),
                                            arg2.getResultPtr(), {iterVal});
  Value *resultPtr = this->builder->CreateGEP(Type::getFloatTy(*this->context),
                                              resultBasePtr, {iterVal});
  Value *arg1Val =
      this->builder->CreateLoad(Type::getFloatTy(*this->context), arg1Ptr);
  Value *arg2Val =
      this->builder->CreateLoad(Type::getFloatTy(*this->context), arg2Ptr);
  Value *processSumVal = this->builder->CreateFAdd(arg1Val, arg2Val);
  this->builder->CreateStore(processSumVal, resultPtr);

  Value *nextIterVal = this->builder->CreateAdd(iterVal, builder->getInt32(1));
  this->builder->CreateStore(nextIterVal, processIterAlloca);
  //   print("nextIterVal: %d", nextIterVal);
  //   print("sumVal: %d", sumVal);

  this->addLoopEnd(processLoopBB, nextIterVal, sumVal);

  // END PROCESS LOOP

  return RValue(resultBasePtr, arg1.getShapePtr(), arg1.getShapeLength());
}

void LlvmCodegen::print(string fmt, Value *val = nullptr) {
  Type *intType = Type::getInt32Ty(*this->context);
  Type *charPtrType = PointerType::getUnqual(Type::getInt8Ty(*this->context));
  FunctionType *printfType = FunctionType::get(intType, {charPtrType}, true);
  FunctionCallee printfFn =
      this->module->getOrInsertFunction("printf", printfType);

  Value *formatStr = this->builder->CreateGlobalString(fmt);
  std::vector<Value *> printfArgs = {formatStr};

  if (val != nullptr)
    printfArgs.push_back(val);
  this->builder->CreateCall(printfFn, printfArgs, "printfCall");
}

void LlvmCodegen::printResultCodegen(RValue returnExpr) {
  print("<");

  AllocaInst *sumAlloca =
      this->builder->CreateAlloca(Type::getInt32Ty(*this->context), nullptr);
  builder->CreateStore(this->builder->getInt32(1), sumAlloca);
  auto [shapeLoopBB, shapeIterAlloca] =
      this->addLoopStart(this->builder->getInt32(0));
  Value *shapeIterVal = this->builder->CreateLoad(
      Type::getInt32Ty(*this->context), shapeIterAlloca);

  Value *shapePtr =
      this->builder->CreateGEP(Type::getInt32Ty(*this->context),
                               returnExpr.getShapePtr(), {shapeIterVal});
  Value *shapeVal =
      this->builder->CreateLoad(Type::getInt32Ty(*this->context), shapePtr);
  Value *oldSumVal =
      this->builder->CreateLoad(Type::getInt32Ty(*this->context), sumAlloca);
  Value *newSumVal = this->builder->CreateMul(oldSumVal, shapeVal);
  builder->CreateStore(newSumVal, sumAlloca);

  print("%dx", shapeVal);

  Value *nextShapeIterVal =
      this->builder->CreateAdd(shapeIterVal, builder->getInt32(1));
  this->builder->CreateStore(nextShapeIterVal, shapeIterAlloca);
  this->addLoopEnd(shapeLoopBB, nextShapeIterVal, returnExpr.getShapeLength());

  print("> [");

  Value *sumVal =
      this->builder->CreateLoad(Type::getInt32Ty(*this->context), sumAlloca);
  auto [processLoopBB, processIterAlloca] =
      this->addLoopStart(this->builder->getInt32(0));
  Value *iterVal = this->builder->CreateLoad(Type::getInt32Ty(*this->context),
                                             processIterAlloca);
  Value *arg1Ptr = this->builder->CreateGEP(
      Type::getFloatTy(*this->context), returnExpr.getResultPtr(), {iterVal});
  Value *arg1Val =
      this->builder->CreateLoad(Type::getFloatTy(*this->context), arg1Ptr);

  print("%.2f ",
        this->builder->CreateFPExt(arg1Val, this->builder->getDoubleTy()));

  Value *nextIterVal = this->builder->CreateAdd(iterVal, builder->getInt32(1));
  this->builder->CreateStore(nextIterVal, processIterAlloca);
  this->addLoopEnd(processLoopBB, nextIterVal, sumVal);

  print("]\n");
  this->builder->CreateRet(returnExpr.getResultPtr());
}

void LlvmCodegen::returnCodegen(Value *returnExpr) {
  this->builder->CreateRet(returnExpr);
}
} // namespace AplCodegen