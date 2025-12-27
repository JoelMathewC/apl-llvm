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
LlvmCodegen::LlvmCodegen(llvm::DataLayout dataLayout) {
  this->dataLayout = dataLayout;
  this->initializeContextAndModule();
}

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

LlvmCodegen::~LlvmCodegen() = default;

Value *LlvmCodegen::literalCodegen(const vector<float> vec) {
  // Add the malloc function (if it doesnt exist).
  FunctionCallee mallocFunc = this->module->getOrInsertFunction(
      Constants::heapAllocFuncName, this->builder->getPtrTy(),
      Type::getInt32Ty(*this->context));

  // Get the size that needs to be reserved and reserve it.
  uint64_t elementSize =
      this->module->getDataLayout().getTypeAllocSize(this->builder->getPtrTy());
  Value *totalSize = ConstantInt::get(Type::getInt32Ty(*this->context),
                                      vec.size() * elementSize);
  Value *rawPtr = this->builder->CreateCall(mallocFunc, totalSize);

  // Create a global variable with the literal constant value.
  ArrayType *arrTy =
      ArrayType::get(Type::getFloatTy(*this->context), vec.size());
  Constant *init =
      ConstantDataArray::get(*this->context, ArrayRef(vec.data(), vec.size()));
  llvm::GlobalVariable *sourceGlobal = new llvm::GlobalVariable(
      *this->module, arrTy, true, llvm::GlobalValue::InternalLinkage, init);

  // Initiate a memcpy of the global constant into the reserved memory location.
  this->builder->CreateMemCpy(rawPtr, MaybeAlign(0), sourceGlobal,
                              MaybeAlign(0), totalSize);
  return rawPtr;
}

Value *LlvmCodegen::addCodegen(Value *arg1, Value *arg2,
                               unsigned long numElem) {
  // Allocate space to store the value of the iterator
  AllocaInst *alloca =
      this->builder->CreateAlloca(Type::getInt32Ty(*this->context), nullptr);
  builder->CreateStore(builder->getInt32(0), alloca);

  // Add an explicit fall through to the loop body block
  Function *func = this->builder->GetInsertBlock()->getParent();
  BasicBlock *LoopBB = BasicBlock::Create(*this->context, "", func);
  this->builder->CreateBr(LoopBB);
  this->builder->SetInsertPoint(LoopBB);

  // Use the iterator to fetch pointers to the current elems of the array
  // being processed.
  Value *currVal =
      this->builder->CreateLoad(Type::getInt32Ty(*this->context), alloca);
  Value *arg1Ptr = this->builder->CreateGEP(Type::getFloatTy(*this->context),
                                            arg1, {currVal});
  Value *arg2Ptr = this->builder->CreateGEP(Type::getFloatTy(*this->context),
                                            arg2, {currVal});

  // Load the elems of arrays, perform addition and store the result in the
  // first array elem location.
  Value *arg1Val =
      this->builder->CreateLoad(Type::getFloatTy(*this->context), arg1Ptr);
  Value *arg2Val =
      this->builder->CreateLoad(Type::getFloatTy(*this->context), arg2Ptr);
  Value *sumVal = this->builder->CreateFAdd(arg1Val, arg2Val);
  this->builder->CreateStore(sumVal, arg1Ptr);

  // Increment the iterator and tests for the exit condition
  Value *nextVal = this->builder->CreateAdd(currVal, builder->getInt32(1));
  this->builder->CreateStore(nextVal, alloca);
  Value *endCond = builder->CreateICmpULT(nextVal, builder->getInt32(numElem));

  // Create a block for execution after the loop
  BasicBlock *AfterLoopBB = BasicBlock::Create(*this->context, "", func);
  this->builder->CreateCondBr(endCond, LoopBB, AfterLoopBB);
  this->builder->SetInsertPoint(AfterLoopBB);

  return arg1;
}

Value *LlvmCodegen::subCodegen(Value *arg1, Value *arg2,
                               unsigned long numElem) {
  // Allocate space to store the value of the iterator
  AllocaInst *alloca =
      this->builder->CreateAlloca(Type::getInt32Ty(*this->context), nullptr);
  builder->CreateStore(builder->getInt32(0), alloca);

  // Add an explicit fall through to the loop body block
  Function *func = this->builder->GetInsertBlock()->getParent();
  BasicBlock *LoopBB = BasicBlock::Create(*this->context, "", func);
  this->builder->CreateBr(LoopBB);
  this->builder->SetInsertPoint(LoopBB);

  // Use the iterator to fetch pointers to the current elems of the array
  // being processed.
  Value *currVal =
      this->builder->CreateLoad(Type::getInt32Ty(*this->context), alloca);
  Value *arg1Ptr = this->builder->CreateGEP(Type::getFloatTy(*this->context),
                                            arg1, {currVal});
  Value *arg2Ptr = this->builder->CreateGEP(Type::getFloatTy(*this->context),
                                            arg2, {currVal});

  // Load the elems of arrays, perform addition and store the result in the
  // first array elem location.
  Value *arg1Val =
      this->builder->CreateLoad(Type::getFloatTy(*this->context), arg1Ptr);
  Value *arg2Val =
      this->builder->CreateLoad(Type::getFloatTy(*this->context), arg2Ptr);
  Value *sumVal = this->builder->CreateFSub(arg1Val, arg2Val);
  this->builder->CreateStore(sumVal, arg1Ptr);

  // Increment the iterator and tests for the exit condition
  Value *nextVal = this->builder->CreateAdd(currVal, builder->getInt32(1));
  this->builder->CreateStore(nextVal, alloca);
  Value *endCond = builder->CreateICmpULT(nextVal, builder->getInt32(numElem));

  // Create a block for execution after the loop
  BasicBlock *AfterLoopBB = BasicBlock::Create(*this->context, "", func);
  this->builder->CreateCondBr(endCond, LoopBB, AfterLoopBB);
  this->builder->SetInsertPoint(AfterLoopBB);

  return arg1;
}

Value *LlvmCodegen::mulCodegen(Value *arg1, Value *arg2,
                               unsigned long numElem) {
  // Allocate space to store the value of the iterator
  AllocaInst *alloca =
      this->builder->CreateAlloca(Type::getInt32Ty(*this->context), nullptr);
  builder->CreateStore(builder->getInt32(0), alloca);

  // Add an explicit fall through to the loop body block
  Function *func = this->builder->GetInsertBlock()->getParent();
  BasicBlock *LoopBB = BasicBlock::Create(*this->context, "", func);
  this->builder->CreateBr(LoopBB);
  this->builder->SetInsertPoint(LoopBB);

  // Use the iterator to fetch pointers to the current elems of the array
  // being processed.
  Value *currVal =
      this->builder->CreateLoad(Type::getInt32Ty(*this->context), alloca);
  Value *arg1Ptr = this->builder->CreateGEP(Type::getFloatTy(*this->context),
                                            arg1, {currVal});
  Value *arg2Ptr = this->builder->CreateGEP(Type::getFloatTy(*this->context),
                                            arg2, {currVal});

  // Load the elems of arrays, perform addition and store the result in the
  // first array elem location.
  Value *arg1Val =
      this->builder->CreateLoad(Type::getFloatTy(*this->context), arg1Ptr);
  Value *arg2Val =
      this->builder->CreateLoad(Type::getFloatTy(*this->context), arg2Ptr);
  Value *sumVal = this->builder->CreateFMul(arg1Val, arg2Val);
  this->builder->CreateStore(sumVal, arg1Ptr);

  // Increment the iterator and tests for the exit condition
  Value *nextVal = this->builder->CreateAdd(currVal, builder->getInt32(1));
  this->builder->CreateStore(nextVal, alloca);
  Value *endCond = builder->CreateICmpULT(nextVal, builder->getInt32(numElem));

  // Create a block for execution after the loop
  BasicBlock *AfterLoopBB = BasicBlock::Create(*this->context, "", func);
  this->builder->CreateCondBr(endCond, LoopBB, AfterLoopBB);
  this->builder->SetInsertPoint(AfterLoopBB);

  return arg1;
}

Value *LlvmCodegen::divCodegen(Value *arg1, Value *arg2,
                               unsigned long numElem) {
  // Allocate space to store the value of the iterator
  AllocaInst *alloca =
      this->builder->CreateAlloca(Type::getInt32Ty(*this->context), nullptr);
  builder->CreateStore(builder->getInt32(0), alloca);

  // Add an explicit fall through to the loop body block
  Function *func = this->builder->GetInsertBlock()->getParent();
  BasicBlock *LoopBB = BasicBlock::Create(*this->context, "", func);
  this->builder->CreateBr(LoopBB);
  this->builder->SetInsertPoint(LoopBB);

  // Use the iterator to fetch pointers to the current elems of the array
  // being processed.
  Value *currVal =
      this->builder->CreateLoad(Type::getInt32Ty(*this->context), alloca);
  Value *arg1Ptr = this->builder->CreateGEP(Type::getFloatTy(*this->context),
                                            arg1, {currVal});
  Value *arg2Ptr = this->builder->CreateGEP(Type::getFloatTy(*this->context),
                                            arg2, {currVal});

  // Load the elems of arrays, perform addition and store the result in the
  // first array elem location.
  Value *arg1Val =
      this->builder->CreateLoad(Type::getFloatTy(*this->context), arg1Ptr);
  Value *arg2Val =
      this->builder->CreateLoad(Type::getFloatTy(*this->context), arg2Ptr);
  Value *sumVal = this->builder->CreateFDiv(arg1Val, arg2Val);
  this->builder->CreateStore(sumVal, arg1Ptr);

  // Increment the iterator and tests for the exit condition
  Value *nextVal = this->builder->CreateAdd(currVal, builder->getInt32(1));
  this->builder->CreateStore(nextVal, alloca);
  Value *endCond = builder->CreateICmpULT(nextVal, builder->getInt32(numElem));

  // Create a block for execution after the loop
  BasicBlock *AfterLoopBB = BasicBlock::Create(*this->context, "", func);
  this->builder->CreateCondBr(endCond, LoopBB, AfterLoopBB);
  this->builder->SetInsertPoint(AfterLoopBB);

  return arg1;
}

pair<unique_ptr<LLVMContext>, unique_ptr<Module>>
LlvmCodegen::getAndReinitializeContextAndModule() {
  auto prev_context = std::move(this->context);
  auto prev_module = std::move(this->module);

  this->initializeContextAndModule();
  return make_pair(std::move(prev_context), std::move(prev_module));
}

void LlvmCodegen::returnCodegen(Value *returnExpr) {
  this->builder->CreateRet(returnExpr);
}
} // namespace AplCodegen