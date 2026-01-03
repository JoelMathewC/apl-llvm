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
// region RValue
Value *RValue::getResultPtr() { return this->resultPtr; }
Value *RValue::getShapePtr() { return this->shapePtr; }
Value *RValue::getShapeLength() { return this->shapeLength; }
RValue::RValue(Value *resultPtr, Value *shapePtr, Value *shapeLength)
    : resultPtr(resultPtr), shapePtr(shapePtr), shapeLength(shapeLength) {}
// endregion RValue

// region LlvmCodegen
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

  this->typeInfo =
      new GlobalVariable(*this->module, Type::getInt8Ty(*this->context), true,
                         GlobalValue::ExternalLinkage, nullptr, "_ZTIi");

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
  FunctionCallee mallocFunc = this->module->getOrInsertFunction(
      Constants::heapAllocFuncName, this->builder->getPtrTy(),
      Type::getInt32Ty(*this->context));

  uint64_t elementSize =
      this->module->getDataLayout().getTypeAllocSize(elemType);

  Value *totalSize = builder->CreateMul(
      ConstantInt::get(Type::getInt32Ty(*this->context), elementSize), size);

  return make_pair(this->builder->CreateCall(mallocFunc, totalSize), totalSize);
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

void LlvmCodegen::throwError() {
  Type *RetType = PointerType::getUnqual(*this->context);
  Type *ArgType = Type::getInt64Ty(*this->context);
  auto allocExceptionFunc = this->module->getOrInsertFunction(
      "__cxa_allocate_exception", RetType, ArgType);

  Type *VoidTy = Type::getVoidTy(*this->context);
  Type *PtrTy = PointerType::getUnqual(*this->context);
  auto throwFunc = this->module->getOrInsertFunction("__cxa_throw", VoidTy,
                                                     PtrTy, PtrTy, PtrTy);

  Value *ExPtr = this->builder->CreateCall(
      allocExceptionFunc,
      {ConstantInt::get(Type::getInt64Ty(*this->context), 4)});

  Value *IntPtr =
      this->builder->CreateBitCast(ExPtr, Type::getInt32Ty(*this->context));
  this->builder->CreateStore(
      ConstantInt::get(Type::getInt32Ty(*this->context), 1), ExPtr);

  //   GlobalVariable *TypeInfo =
  //       new GlobalVariable(*this->module, Type::getInt8Ty(*this->context),
  //       true,
  //                          GlobalValue::ExternalLinkage, nullptr, "_ZTIi");

  Value *NullPtr = ConstantPointerNull::get(builder->getPtrTy());
  builder->CreateCall(throwFunc, {ExPtr, this->typeInfo, NullPtr});
  this->builder->CreateUnreachable();
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

Value *LlvmCodegen::sumArrShape(RValue arg) {
  // Reserving a space to store total element count
  AllocaInst *totalElemCountAlloca =
      this->builder->CreateAlloca(Type::getInt32Ty(*this->context), nullptr);
  this->builder->CreateStore(this->builder->getInt32(1), totalElemCountAlloca);

  // Loop1: Iterate through the shape array and print shape
  // and count the total number of elements.
  auto [shapeLoopBB, shapeIterAlloca] =
      this->addLoopStart(this->builder->getInt32(0));

  Value *shapeIterVal = this->builder->CreateLoad(
      Type::getInt32Ty(*this->context), shapeIterAlloca);

  Value *shapePtr = this->builder->CreateGEP(Type::getInt32Ty(*this->context),
                                             arg.getShapePtr(), {shapeIterVal});
  Value *shapeVal =
      this->builder->CreateLoad(Type::getInt32Ty(*this->context), shapePtr);

  Value *oldTotalElemCount = this->builder->CreateLoad(
      Type::getInt32Ty(*this->context), totalElemCountAlloca);

  Value *newTotalElemCount =
      this->builder->CreateMul(oldTotalElemCount, shapeVal);

  builder->CreateStore(newTotalElemCount, totalElemCountAlloca);

  Value *nextShapeIterVal =
      this->builder->CreateAdd(shapeIterVal, builder->getInt32(1));

  this->builder->CreateStore(nextShapeIterVal, shapeIterAlloca);

  this->addLoopEnd(shapeLoopBB, nextShapeIterVal, arg.getShapeLength());
  // end Loop 1

  return newTotalElemCount;
}

void LlvmCodegen::verifyDyadicOperands(RValue arg1, RValue arg2) {
  Function *func = this->builder->GetInsertBlock()->getParent();
  BasicBlock *ShapeSizeVerifyFailedBB =
      BasicBlock::Create(*this->context, "", func);
  BasicBlock *ShapeSizeVerifyPassedBB =
      BasicBlock::Create(*this->context, "", func);
  BasicBlock *ShapeValVerifyFailedBB =
      BasicBlock::Create(*this->context, "", func);
  BasicBlock *ShapeValVerifyPassedBB =
      BasicBlock::Create(*this->context, "", func);

  Value *verifyCond1 =
      builder->CreateICmpEQ(arg1.getShapeLength(), arg2.getShapeLength());

  this->builder->CreateCondBr(verifyCond1, ShapeSizeVerifyPassedBB,
                              ShapeSizeVerifyFailedBB);

  // Shape Size Verify Failed
  this->builder->SetInsertPoint(ShapeSizeVerifyFailedBB);
  throwError();

  // Shape Size Verify Passed
  this->builder->SetInsertPoint(ShapeSizeVerifyPassedBB);

  auto [shapeLoopBB, shapeIterAlloca] =
      this->addLoopStart(this->builder->getInt32(0));

  Value *shapeIterVal = this->builder->CreateLoad(
      Type::getInt32Ty(*this->context), shapeIterAlloca);

  Value *shapePtr1 = this->builder->CreateGEP(
      Type::getInt32Ty(*this->context), arg1.getShapePtr(), {shapeIterVal});
  Value *shapeVal1 =
      this->builder->CreateLoad(Type::getInt32Ty(*this->context), shapePtr1);

  Value *shapePtr2 = this->builder->CreateGEP(
      Type::getInt32Ty(*this->context), arg2.getShapePtr(), {shapeIterVal});
  Value *shapeVal2 =
      this->builder->CreateLoad(Type::getInt32Ty(*this->context), shapePtr2);

  Value *verifyCond2 = builder->CreateICmpEQ(shapeVal1, shapeVal2);

  this->builder->CreateCondBr(verifyCond2, ShapeValVerifyPassedBB,
                              ShapeValVerifyFailedBB);

  // Shape Val Verify Failed
  this->builder->SetInsertPoint(ShapeValVerifyFailedBB);
  throwError();

  this->builder->SetInsertPoint(ShapeValVerifyPassedBB);
  Value *nextShapeIterVal =
      this->builder->CreateAdd(shapeIterVal, builder->getInt32(1));
  this->builder->CreateStore(nextShapeIterVal, shapeIterAlloca);
  this->addLoopEnd(shapeLoopBB, nextShapeIterVal, arg1.getShapeLength());
}

void LlvmCodegen::printResultCodegen(RValue returnExpr) {
  print("<");

  // Reserving a space to store total element count
  AllocaInst *totalElemCountAlloca =
      this->builder->CreateAlloca(Type::getInt32Ty(*this->context), nullptr);
  builder->CreateStore(this->builder->getInt32(1), totalElemCountAlloca);

  // Loop1: Iterate through the shape array and print shape
  // and count the total number of elements.
  auto [shapeLoopBB, shapeIterAlloca] =
      this->addLoopStart(this->builder->getInt32(0));

  Value *shapeIterVal = this->builder->CreateLoad(
      Type::getInt32Ty(*this->context), shapeIterAlloca);

  Value *shapePtr =
      this->builder->CreateGEP(Type::getInt32Ty(*this->context),
                               returnExpr.getShapePtr(), {shapeIterVal});
  Value *shapeVal =
      this->builder->CreateLoad(Type::getInt32Ty(*this->context), shapePtr);

  Value *oldTotalElemCount = this->builder->CreateLoad(
      Type::getInt32Ty(*this->context), totalElemCountAlloca);

  Value *newTotalElemCount =
      this->builder->CreateMul(oldTotalElemCount, shapeVal);

  builder->CreateStore(newTotalElemCount, totalElemCountAlloca);

  print("%dx", shapeVal);

  Value *nextShapeIterVal =
      this->builder->CreateAdd(shapeIterVal, builder->getInt32(1));

  this->builder->CreateStore(nextShapeIterVal, shapeIterAlloca);

  this->addLoopEnd(shapeLoopBB, nextShapeIterVal, returnExpr.getShapeLength());
  // end Loop 1

  print("> [");

  // Loop 2: Loop through the array and print individual values
  auto [processLoopBB, processIterAlloca] =
      this->addLoopStart(this->builder->getInt32(0));

  Value *iterVal = this->builder->CreateLoad(Type::getInt32Ty(*this->context),
                                             processIterAlloca);

  Value *arg1Ptr = this->builder->CreateGEP(
      Type::getFloatTy(*this->context), returnExpr.getResultPtr(), {iterVal});

  Value *arg1Val =
      this->builder->CreateLoad(Type::getFloatTy(*this->context), arg1Ptr);

  // https://stackoverflow.com/questions/63144506/printf-doesnt-work-for-floats-in-llvm-ir
  print("%.2f ",
        this->builder->CreateFPExt(arg1Val, this->builder->getDoubleTy()));

  Value *nextIterVal = this->builder->CreateAdd(iterVal, builder->getInt32(1));
  this->builder->CreateStore(nextIterVal, processIterAlloca);
  this->addLoopEnd(processLoopBB, nextIterVal, newTotalElemCount);
  // end Loop 2

  print("]\n");
  this->builder->CreateRet(nullptr);
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

RValue LlvmCodegen::addCodegen(RValue arg1, RValue arg2) {
  // Verify that the operands are of the right size
  verifyDyadicOperands(arg1, arg2);

  // Allocate space for the result
  Value *totalElemCount = sumArrShape(arg1);

  auto [resultBasePtr, resultSize] =
      allocHeap(totalElemCount, this->builder->getFloatTy());

  // Process the result
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

  Value *processVal = this->builder->CreateFAdd(arg1Val, arg2Val);
  this->builder->CreateStore(processVal, resultPtr);

  Value *nextIterVal = this->builder->CreateAdd(iterVal, builder->getInt32(1));
  this->builder->CreateStore(nextIterVal, processIterAlloca);
  this->addLoopEnd(processLoopBB, nextIterVal, totalElemCount);

  return RValue(resultBasePtr, arg1.getShapePtr(), arg1.getShapeLength());
}
// endregion LlvmCodegen
} // namespace AplCodegen