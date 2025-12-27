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

unique_ptr<AplOp::DyadicOp> createDyadicOp(char op) {
  // The op needs to be case to string since some glyphs cannot be
  // represented as chars
  string op_str = string(1, op);
  if (op_str == "+")
    return make_unique<AplOp::AddOp>();
  else if (op_str == "-")
    return make_unique<AplOp::SubOp>();
  else if (op_str == "x")
    return make_unique<AplOp::MulOp>();
  else if (op_str == "÷")
    return make_unique<AplOp::DivOp>();
  else
    throw std::logic_error("Operation " + op_str + " is unimplemented!");
}
} // namespace AplOp