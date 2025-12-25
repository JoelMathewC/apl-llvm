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
}

LlvmCodegen::LlvmCodegen(llvm::DataLayout dataLayout) {
  this->dataLayout = dataLayout;
  this->initializeContextAndModule();
}

LlvmCodegen::~LlvmCodegen() = default;

Value *LlvmCodegen::literalCodegen(const float *vecPtr, int size) {
  return ConstantDataVector::get(*this->context, ArrayRef(vecPtr, size));
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

Value *LlvmCodegen::callCodegen() { return 0; }

Value *LlvmCodegen::assignCodegen() { return 0; }

pair<unique_ptr<LLVMContext>, unique_ptr<Module>>
LlvmCodegen::getAndReinitializeContextAndModule() {
  auto prev_context = std::move(this->context);
  auto prev_module = std::move(this->module);

  this->initializeContextAndModule();
  return make_pair(std::move(prev_context), std::move(prev_module));
}

Function *LlvmCodegen::wrapInAnonymousFunction(Value *exprIR) {
  FunctionType *FT = FunctionType::get(Type::getDoubleTy(*this->context),
                                       std::vector<Type *>(), false);
  Function *F =
      Function::Create(FT, Function::ExternalLinkage,
                       Constants::anonymousExprName, this->module.get());
  BasicBlock *BB =
      BasicBlock::Create(*this->context, Constants::basicBlockEntryTag, F);
  this->builder->SetInsertPoint(BB);
  this->builder->CreateRet(exprIR);
  return F;
}
} // namespace AplCodegen