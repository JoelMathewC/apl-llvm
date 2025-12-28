#include "op.hpp"
#include <string>
#include <vector>

using namespace std;

namespace AplOp {
// Local Function
unsigned long getNumElemFromShape(vector<unsigned long> resultShape) {
  unsigned long numElem = 1;
  for (auto idx : resultShape)
    numElem *= idx;
  return numElem;
}
// End local Function

bool DyadicOp::isOperandShapeCorrect(
    vector<unsigned long> firstOperandShape,
    vector<unsigned long> secondOperandShape) const {
  return false;
}

const vector<unsigned long>
DyadicOp::getResultShape(vector<unsigned long> firstOperandShape,
                         vector<unsigned long> secondOperandShape) const {
  return {};
}

bool ShapeRetainingDyadicOp::isOperandShapeCorrect(
    vector<unsigned long> firstOperandShape,
    vector<unsigned long> secondOperandShape) const {
  return firstOperandShape == secondOperandShape;
}

const vector<unsigned long> ShapeRetainingDyadicOp::getResultShape(
    vector<unsigned long> firstOperandShape,
    vector<unsigned long> secondOperandShape) const {
  return firstOperandShape;
}

const string Op::print() const { return "unspecified-op"; }

Value *DyadicOp::codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                         Value *rhs, vector<unsigned long> resultShape) {
  return nullptr;
}

const string AddOp::print() const { return "+"; }

Value *AddOp::codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                      Value *rhs, vector<unsigned long> resultShape) {
  return codegenManager->addCodegen(lhs, rhs, getNumElemFromShape(resultShape));
}

const string SubOp::print() const { return "-"; }

Value *SubOp::codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                      Value *rhs, vector<unsigned long> resultShape) {
  return codegenManager->subCodegen(lhs, rhs, getNumElemFromShape(resultShape));
}

const string MulOp::print() const { return "x"; }

Value *MulOp::codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                      Value *rhs, vector<unsigned long> resultShape) {
  return codegenManager->mulCodegen(lhs, rhs, getNumElemFromShape(resultShape));
}

const string DivOp::print() const { return "÷"; }

Value *DivOp::codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                      Value *rhs, vector<unsigned long> resultShape) {
  return codegenManager->divCodegen(lhs, rhs, getNumElemFromShape(resultShape));
}

unique_ptr<AplOp::DyadicOp> createDyadicOp(Symbol op) {
  switch (op) {
  case Symbol::PLUS:
    return make_unique<AplOp::AddOp>();
  case Symbol::MINUS:
    return make_unique<AplOp::SubOp>();
  case Symbol::TIMES:
    return make_unique<AplOp::MulOp>();
  case Symbol::DIVIDE:
    return make_unique<AplOp::DivOp>();
  default:
    throw std::logic_error("Operation is unimplemented!");
  }
}
} // namespace AplOp