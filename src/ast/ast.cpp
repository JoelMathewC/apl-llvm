#include "ast.hpp"
#include "op.hpp"
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "../codegen/codegen.hpp"
#include "llvm/IR/IRBuilder.h"

using namespace std;

namespace AplAst {
AplCodegen::RValue Node::codegen(AplCodegen::LlvmCodegen *codegenManager) {
  return AplCodegen::RValue(nullptr, nullptr, nullptr);
}

const string Node::print() const { return "unspecialized-node"; }
// End Node Section

// Literal section
Literal::Literal(const vector<float> val) : val(val) {}

unique_ptr<Literal> Literal::create(float val) {
  vector<float> vec = {val};
  return make_unique<Literal>(vec);
}

unique_ptr<Literal> Literal::create(vector<float> vec, float new_elem) {
  vec.push_back(new_elem);
  return make_unique<Literal>(vec);
}

const vector<float> &Literal::getVal() const { return this->val; }

AplCodegen::RValue Literal::codegen(AplCodegen::LlvmCodegen *codegenManager) {
  return codegenManager->literalCodegen(this->val);
}

const string Literal::print() const {
  // generate comma seperated string of array
  string vec_str = "";
  for (auto &elem : this->val) {
    vec_str += to_string(elem);
    vec_str += ",";
  }

  // remove the trailing comma
  vec_str.pop_back();

  return "LITERAL(" + vec_str + ")";
}
// end Literal section

// MonadicCall section
MonadicCall::MonadicCall(unique_ptr<AplOp::MonadicOp> op, unique_ptr<Node> arg)
    : op(std::move(op)), arg(std::move(arg)) {}

unique_ptr<MonadicCall> MonadicCall::create(AplOp::Symbol op,
                                            unique_ptr<Node> &arg) {
  auto monadicOp = AplOp::createMonadicOp(op);
  return make_unique<MonadicCall>(std::move(monadicOp), std::move(arg));
}

AplCodegen::RValue
MonadicCall::codegen(AplCodegen::LlvmCodegen *codegenManager) {
  return this->op->codegen(codegenManager, this->arg->codegen(codegenManager));
}

const string MonadicCall::print() const {
  return "MonadicCall(" + this->op->print() + "," + this->arg->print() + ")";
}
// end MonadicCall section

// DyadicCall section
DyadicCall::DyadicCall(unique_ptr<AplOp::DyadicOp> op, unique_ptr<Node> arg1,
                       unique_ptr<Node> arg2)
    : op(std::move(op)), arg1(std::move(arg1)), arg2(std::move(arg2)) {}

unique_ptr<DyadicCall> DyadicCall::create(AplOp::Symbol op,
                                          unique_ptr<Node> &arg1,
                                          unique_ptr<Node> &arg2) {
  auto dyadicOp = AplOp::createDyadicOp(op);
  return make_unique<DyadicCall>(std::move(dyadicOp), std::move(arg1),
                                 std::move(arg2));
}

AplCodegen::RValue
DyadicCall::codegen(AplCodegen::LlvmCodegen *codegenManager) {
  return this->op->codegen(codegenManager, this->arg1->codegen(codegenManager),
                           this->arg2->codegen(codegenManager));
}

const string DyadicCall::print() const {
  return "DyadicCall(" + this->op->print() + "," + this->arg1->print() + "," +
         this->arg2->print() + ")";
}
// end DyadicCall section

// Misc. section
ostream &operator<<(ostream &os, const Node &node) {
  return os << node.print();
}
// end Misc. section
} // namespace AplAst