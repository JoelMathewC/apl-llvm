#include "codegen.hpp"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <iostream>
#include <map>

using namespace std;
using namespace llvm;

namespace AplCodegen {
LlvmCodegen::LlvmCodegen() {
  this->context = make_unique<LLVMContext>();
  this->module = make_unique<Module>("APL JIT", *this->context);
  this->builder = make_unique<IRBuilder<>>(*this->context);
}

LlvmCodegen::~LlvmCodegen() = default;

Value *LlvmCodegen::literalCodegen(const vector<float> vec) {
  return ConstantFP::get(*this->context, APFloat(vec[0]));
}

Value *LlvmCodegen::variableCodegen(string name) {
  Value *variable_val = this->variableMap[name];

  // TODO: replace with call to exception handler
  if (!variable_val) {
    cout << "Variable not defined: " << name << "\n";
    return nullptr;
  }

  return variable_val;
}

Value *LlvmCodegen::callCodegen() { return 0; }

Value *LlvmCodegen::assignCodegen() { return 0; }

unique_ptr<LLVMContext> LlvmCodegen::getContext() {
  return std::move(this->context);
}
unique_ptr<Module> LlvmCodegen::getModule() { return std::move(this->module); }

Function *LlvmCodegen::wrapInAnonymousFunction(Value *exprIR) {
  FunctionType *FT = FunctionType::get(Type::getDoubleTy(*this->context),
                                       std::vector<Type *>(), false);
  Function *F = Function::Create(FT, Function::ExternalLinkage, "__anon_expr",
                                 this->module.get());
  BasicBlock *BB = BasicBlock::Create(*this->context, "entry", F);
  this->builder->SetInsertPoint(BB);
  this->builder->CreateRet(exprIR);
  return F;
}
} // namespace AplCodegen