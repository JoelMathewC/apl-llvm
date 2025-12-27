#include "ast.hpp"
#include "op.hpp"
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

#include "../codegen/codegen.hpp"
#include "llvm/IR/IRBuilder.h"

using namespace std;

namespace AplAst {
// Node section
Node::Node(const vector<unsigned long> shape) : shape(shape) {}

Node::~Node() = default;

const vector<unsigned long> Node::getShape() { return this->shape; }

llvm::Value *Node::codegen(AplCodegen::LlvmCodegen *codegenManager) {
  return nullptr;
}

const string Node::print() const { return "unspecialized-node"; }
// End Node Section

// Literal section
Literal::Literal(const vector<float> val)
    : val(val), Node(vector<unsigned long>{val.size()}) {}

unique_ptr<Literal> Literal::create(float val) {
  vector<float> vec = {val};
  return make_unique<Literal>(vec);
}

unique_ptr<Literal> Literal::create(vector<float> vec, float new_elem) {
  vec.push_back(new_elem);
  return make_unique<Literal>(vec);
}

const vector<float> &Literal::getVal() const { return this->val; }

llvm::Value *Literal::codegen(AplCodegen::LlvmCodegen *codegenManager) {
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

// DyadicCall section
DyadicCall::DyadicCall(unique_ptr<AplOp::DyadicOp> op, unique_ptr<Node> arg1,
                       unique_ptr<Node> arg2)
    : Node(op->getResultShape(arg1->getShape(), arg2->getShape())),
      op(std::move(op)), arg1(std::move(arg1)), arg2(std::move(arg2)) {}

unique_ptr<DyadicCall> DyadicCall::create(char op, unique_ptr<Node> &arg1,
                                          unique_ptr<Node> &arg2) {
  auto dyadicOp = AplOp::createDyadicOp(op);
  if (!dyadicOp->isOperandShapeCorrect(arg1->getShape(), arg2->getShape())) {
    throw std::invalid_argument("Syntax Error: mismatched argument shapes!");
  }
  return make_unique<DyadicCall>(std::move(dyadicOp), std::move(arg1),
                                 std::move(arg2));
}

llvm::Value *DyadicCall::codegen(AplCodegen::LlvmCodegen *codegenManager) {
  return this->op->codegen(codegenManager, this->arg1->codegen(codegenManager),
                           this->arg2->codegen(codegenManager),
                           this->getShape());
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