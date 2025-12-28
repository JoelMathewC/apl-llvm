#include "op.hpp"
#include <string>
#include <vector>

using namespace std;

namespace AplOp {
// region LocalFunction
unsigned long getNumElemFromShape(vector<unsigned long> resultShape) {
  unsigned long numElem = 1;
  for (auto idx : resultShape)
    numElem *= idx;
  return numElem;
}
// endregion LocalFunction

// region MonadicOp
const vector<unsigned long>
MonadicOp::getResultShape(vector<unsigned long> operandShape) const {
  return {};
}

Value *MonadicOp::codegen(AplCodegen::LlvmCodegen *codegenManager,
                          Value *operand, vector<unsigned long> resultShape) {
  return nullptr;
}

const vector<unsigned long> ShapeRetainingMonadicOp::getResultShape(
    vector<unsigned long> operandShape) const {
  return operandShape;
}

Value *NegateOp::codegen(AplCodegen::LlvmCodegen *codegenManager,
                         Value *operand, vector<unsigned long> resultShape) {
  return codegenManager->negateCodegen(operand,
                                       getNumElemFromShape(resultShape));
}
// endregion MonadicOp

// region DyadicOp
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

Value *DyadicOp::codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                         Value *rhs, vector<unsigned long> resultShape) {
  return nullptr;
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

Value *AddOp::codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                      Value *rhs, vector<unsigned long> resultShape) {
  return codegenManager->addCodegen(lhs, rhs, getNumElemFromShape(resultShape));
}

Value *SubOp::codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                      Value *rhs, vector<unsigned long> resultShape) {
  return codegenManager->subCodegen(lhs, rhs, getNumElemFromShape(resultShape));
}

Value *MulOp::codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                      Value *rhs, vector<unsigned long> resultShape) {
  return codegenManager->mulCodegen(lhs, rhs, getNumElemFromShape(resultShape));
}

Value *DivOp::codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                      Value *rhs, vector<unsigned long> resultShape) {
  return codegenManager->divCodegen(lhs, rhs, getNumElemFromShape(resultShape));
}
// endregion DyadicOp

// region HelperMethods
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
    throw std::logic_error("Specified dyadic operation is unimplemented!");
  }
}

unique_ptr<AplOp::MonadicOp> createMonadicOp(Symbol op) {
  switch (op) {
  case Symbol::MINUS:
    return make_unique<AplOp::NegateOp>();
  default:
    throw std::logic_error("Specified monadic operation is unimplemented!");
  }
}
// endregion HelperMethods

const string Op::print() const { return "unspecified-op"; }
const string NegateOp::print() const { return "-"; }
const string AddOp::print() const { return "+"; }
const string SubOp::print() const { return "-"; }
const string MulOp::print() const { return "x"; }
const string DivOp::print() const { return "÷"; }
} // namespace AplOp