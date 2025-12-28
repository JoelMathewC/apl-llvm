#pragma once

#include "../codegen/codegen.hpp"
#include "llvm/IR/IRBuilder.h"
#include <string>

using namespace std;
using namespace llvm;

namespace AplOp {
enum Symbol { PLUS, MINUS, TIMES, DIVIDE };

class Op {
public:
  virtual const string print() const;
};

// region MonadicOp
class MonadicOp : public Op {
public:
  virtual const vector<unsigned long>
  getResultShape(vector<unsigned long> operandShape) const;
  virtual Value *codegen(AplCodegen::LlvmCodegen *codegenManager,
                         Value *operand, vector<unsigned long> resultShape);
};

class ShapeRetainingMonadicOp : public MonadicOp {
public:
  const vector<unsigned long>
  getResultShape(vector<unsigned long> operandShape) const override;
};

class NegateOp : public ShapeRetainingMonadicOp {
public:
  Value *codegen(AplCodegen::LlvmCodegen *codegenManager, Value *operand,
                 vector<unsigned long> resultShape) override;
  const string print() const override;
};
// endregion MonadicOp

// region DyadicOp
class DyadicOp : public Op {
public:
  virtual bool
  isOperandShapeCorrect(vector<unsigned long> firstOperandShape,
                        vector<unsigned long> secondOperandShape) const;
  virtual const vector<unsigned long>
  getResultShape(vector<unsigned long> firstOperandShape,
                 vector<unsigned long> secondOperandShape) const;
  virtual Value *codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                         Value *rhs, vector<unsigned long> resultShape);
};

class ShapeRetainingDyadicOp : public DyadicOp {
public:
  bool isOperandShapeCorrect(
      vector<unsigned long> firstOperandShape,
      vector<unsigned long> secondOperandShape) const override;
  const vector<unsigned long>
  getResultShape(vector<unsigned long> firstOperandShape,
                 vector<unsigned long> secondOperandShape) const override;
};

class AddOp : public ShapeRetainingDyadicOp {
public:
  Value *codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                 Value *rhs, vector<unsigned long> resultShape) override;
  const string print() const override;
};

class SubOp : public ShapeRetainingDyadicOp {
public:
  Value *codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                 Value *rhs, vector<unsigned long> resultShape) override;
  const string print() const override;
};

class MulOp : public ShapeRetainingDyadicOp {
public:
  Value *codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                 Value *rhs, vector<unsigned long> resultShape) override;
  const string print() const override;
};

class DivOp : public ShapeRetainingDyadicOp {
public:
  Value *codegen(AplCodegen::LlvmCodegen *codegenManager, Value *lhs,
                 Value *rhs, vector<unsigned long> resultShape) override;
  const string print() const override;
};
// endregion DyadicOp

// region HelperMethods
unique_ptr<DyadicOp> createDyadicOp(Symbol op);
unique_ptr<MonadicOp> createMonadicOp(Symbol op);
// endregion HelperMethods
} // namespace AplOp