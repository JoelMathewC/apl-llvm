#include "op.hpp"
#include <string>
#include <vector>

using namespace std;

namespace AplOp {
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
  unsigned long numElem = 1;
  for (auto idx : resultShape)
    numElem *= idx;
  return codegenManager->addCodegen(lhs, rhs, numElem);
}

const string SubOp::print() const { return "-"; }

Value *SubOp::codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                      Value *rhs, vector<unsigned long> resultShape) {
  return nullptr;
}

const string MulOp::print() const { return "x"; }

Value *MulOp::codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                      Value *rhs, vector<unsigned long> resultShape) {
  return nullptr;
}

const string ExpOp::print() const { return "*"; }

Value *ExpOp::codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                      Value *rhs, vector<unsigned long> resultShape) {
  return nullptr;
}

const string DivOp::print() const { return "÷"; }

Value *DivOp::codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                      Value *rhs, vector<unsigned long> resultShape) {
  return nullptr;
}
} // namespace AplOp