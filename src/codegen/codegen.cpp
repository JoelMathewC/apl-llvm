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
void LlvmCodegen::initializeContextAndModule() {
  this->context = make_unique<LLVMContext>();
  this->module = make_unique<Module>(Constants::moduleName, *this->context);
  this->module->setDataLayout(this->dataLayout);
  this->builder = make_unique<IRBuilder<>>(*this->context);

  FunctionType *FT = FunctionType::get(this->builder->getPtrTy(),
                                       std::vector<Type *>(), false);
  Function *F =
      Function::Create(FT, Function::ExternalLinkage,
                       Constants::anonymousExprName, this->module.get());
  BasicBlock *BB =
      BasicBlock::Create(*this->context, Constants::basicBlockEntryTag, F);
  this->builder->SetInsertPoint(BB);
}

LlvmCodegen::LlvmCodegen(llvm::DataLayout dataLayout) {
  this->dataLayout = dataLayout;
  this->initializeContextAndModule();
}

LlvmCodegen::~LlvmCodegen() = default;

Value *LlvmCodegen::literalCodegen(const vector<float> vec) {
  FunctionCallee mallocFunc = this->module->getOrInsertFunction(
      "malloc", this->builder->getPtrTy(), Type::getInt32Ty(*this->context));

  uint64_t elementSize =
      this->module->getDataLayout().getTypeAllocSize(this->builder->getPtrTy());
  Value *totalSize = ConstantInt::get(Type::getInt32Ty(*this->context),
                                      vec.size() * elementSize);
  Value *rawPtr =
      this->builder->CreateCall(mallocFunc, totalSize, "heap_array");

  ArrayType *arrTy =
      ArrayType::get(Type::getFloatTy(*this->context), vec.size());
  Constant *init =
      ConstantDataArray::get(*this->context, ArrayRef(vec.data(), vec.size()));
  llvm::GlobalVariable *sourceGlobal = new llvm::GlobalVariable(
      *this->module, arrTy, true, llvm::GlobalValue::InternalLinkage, init,
      "src_data");

  this->builder->CreateMemCpy(rawPtr, MaybeAlign(0), sourceGlobal,
                              MaybeAlign(0), totalSize);
  return rawPtr;
}

Value *LlvmCodegen::variableCodegen(string name) {
  Value *varValue = this->variableMap[name];

  // TODO: replace with call to exception handler
  if (!varValue) {
    cout << "Variable not defined: " << name << "\n";
    return nullptr;
  }

  return varValue;
}

Value *LlvmCodegen::addCodegen(Value *arg1, Value *arg2) {
  Function *func = this->builder->GetInsertBlock()->getParent();

  AllocaInst *alloca = this->builder->CreateAlloca(
      Type::getInt32Ty(*this->context), nullptr, "iterator");
  builder->CreateStore(builder->getInt32(0), alloca);

  // Add an explicit fall through to the LoopBB block
  BasicBlock *LoopBB = BasicBlock::Create(*this->context, "loop", func);
  this->builder->CreateBr(LoopBB);
  this->builder->SetInsertPoint(LoopBB);

  Value *currVal = this->builder->CreateLoad(Type::getInt32Ty(*this->context),
                                             alloca, "iterator");

  Value *arg1Ptr = this->builder->CreateGEP(Type::getFloatTy(*this->context),
                                            arg1, {currVal}, "arg1_ptr");
  Value *arg2Ptr = this->builder->CreateGEP(Type::getFloatTy(*this->context),
                                            arg2, {currVal}, "arg2_ptr");

  Value *arg1Val = this->builder->CreateLoad(Type::getFloatTy(*this->context),
                                             arg1Ptr, "arg1_val");
  Value *arg2Val = this->builder->CreateLoad(Type::getFloatTy(*this->context),
                                             arg2Ptr, "arg2_val");
  Value *sumVal = this->builder->CreateFAdd(arg1Val, arg2Val);
  this->builder->CreateStore(sumVal, arg1Ptr);

  Value *nextVal = this->builder->CreateAdd(currVal, builder->getInt32(1));
  this->builder->CreateStore(nextVal, alloca);

  Value *endCond =
      builder->CreateICmpULT(nextVal, builder->getInt32(5), "loopcond");

  BasicBlock *AfterLoopBB =
      BasicBlock::Create(*this->context, "after-loop", func);
  this->builder->CreateCondBr(endCond, LoopBB, AfterLoopBB);
  this->builder->SetInsertPoint(AfterLoopBB);

  return arg1;
}

Value *LlvmCodegen::callCodegen() { return 0; }

pair<unique_ptr<LLVMContext>, unique_ptr<Module>>
LlvmCodegen::getAndReinitializeContextAndModule() {
  auto prev_context = std::move(this->context);
  auto prev_module = std::move(this->module);

  this->initializeContextAndModule();
  return make_pair(std::move(prev_context), std::move(prev_module));
}

Function *LlvmCodegen::wrapInAnonymousFunction(Value *exprIR) {
  this->builder->CreateRet(exprIR);
  return this->builder->GetInsertBlock()->getParent();
}
} // namespace AplCodegen