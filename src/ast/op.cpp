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
AplCodegen::RValue MonadicOp::codegen(AplCodegen::LlvmCodegen *codegenManager,
                                      AplCodegen::RValue operand) {
  return AplCodegen::RValue(nullptr, nullptr, nullptr);
}

AplCodegen::RValue NegateOp::codegen(AplCodegen::LlvmCodegen *codegenManager,
                                     AplCodegen::RValue operand) {
  return codegenManager->negateCodegen(operand);
}

AplCodegen::RValue IndexGenOp::codegen(AplCodegen::LlvmCodegen *codegenManager,
                                       AplCodegen::RValue operand) {
  return codegenManager->indexGenCodegen(operand);
}
// endregion MonadicOp

// region DyadicOp
AplCodegen::RValue DyadicOp::codegen(AplCodegen::LlvmCodegen *codegenManager,
                                     AplCodegen::RValue lhs,
                                     AplCodegen::RValue rhs) {
  return AplCodegen::RValue(nullptr, nullptr, nullptr);
}

AplCodegen::RValue AddOp::codegen(AplCodegen::LlvmCodegen *codegenManager,
                                  AplCodegen::RValue lhs,
                                  AplCodegen::RValue rhs) {
  return codegenManager->addCodegen(lhs, rhs);
}

AplCodegen::RValue SubOp::codegen(AplCodegen::LlvmCodegen *codegenManager,
                                  AplCodegen::RValue lhs,
                                  AplCodegen::RValue rhs) {
  return codegenManager->subCodegen(lhs, rhs);
}

AplCodegen::RValue MulOp::codegen(AplCodegen::LlvmCodegen *codegenManager,
                                  AplCodegen::RValue lhs,
                                  AplCodegen::RValue rhs) {
  return codegenManager->mulCodegen(lhs, rhs);
}

AplCodegen::RValue DivOp::codegen(AplCodegen::LlvmCodegen *codegenManager,
                                  AplCodegen::RValue lhs,
                                  AplCodegen::RValue rhs) {
  return codegenManager->divCodegen(lhs, rhs);
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
  case Symbol::IOTA:
    return make_unique<AplOp::IndexGenOp>();
  default:
    throw std::logic_error("Specified monadic operation is unimplemented!");
  }
}
// endregion HelperMethods

const string Op::print() const { return "unspecified-op"; }
const string NegateOp::print() const { return "-"; }
const string IndexGenOp::print() const { return "⍳"; }
const string AddOp::print() const { return "+"; }
const string SubOp::print() const { return "-"; }
const string MulOp::print() const { return "x"; }
const string DivOp::print() const { return "÷"; }
} // namespace AplOp