#pragma once

#include "../codegen/codegen.hpp"
#include "llvm/IR/IRBuilder.h"
#include <string>

using namespace std;
using namespace llvm;

namespace AplOp {
class Op {
public:
  virtual const string print() const;
};

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

unique_ptr<DyadicOp> createDyadicOp(char op);
} // namespace AplOp