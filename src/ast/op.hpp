#pragma once

#include "../codegen/codegen.hpp"
#include "llvm/IR/IRBuilder.h"
#include <string>

using namespace std;
using namespace llvm;

namespace AplOp {
enum Symbol { PLUS, MINUS, TIMES, DIVIDE, IOTA, RHO };

class Op {
public:
  virtual const string print() const;
};

// region MonadicOp
class MonadicOp : public Op {
public:
  virtual AplCodegen::RValue codegen(AplCodegen::LlvmCodegen *codegenManager,
                                     AplCodegen::RValue operand);
};

class NegateOp : public MonadicOp {
public:
  AplCodegen::RValue codegen(AplCodegen::LlvmCodegen *codegenManager,
                             AplCodegen::RValue operand) override;
  const string print() const override;
};

class IndexGenOp : public MonadicOp {
public:
  AplCodegen::RValue codegen(AplCodegen::LlvmCodegen *codegenManager,
                             AplCodegen::RValue operand) override;
  const string print() const override;
};
// endregion MonadicOp

// region DyadicOp
class DyadicOp : public Op {
public:
  virtual AplCodegen::RValue codegen(AplCodegen::LlvmCodegen *codegenManager,
                                     AplCodegen::RValue lhs,
                                     AplCodegen::RValue rhs);
};

class AddOp : public DyadicOp {
public:
  AplCodegen::RValue codegen(AplCodegen::LlvmCodegen *codegenManager,
                             AplCodegen::RValue lhs,
                             AplCodegen::RValue rhs) override;
  const string print() const override;
};

class SubOp : public DyadicOp {
public:
  AplCodegen::RValue codegen(AplCodegen::LlvmCodegen *codegenManager,
                             AplCodegen::RValue lhs,
                             AplCodegen::RValue rhs) override;
  const string print() const override;
};

class MulOp : public DyadicOp {
public:
  AplCodegen::RValue codegen(AplCodegen::LlvmCodegen *codegenManager,
                             AplCodegen::RValue lhs,
                             AplCodegen::RValue rhs) override;
  const string print() const override;
};

class DivOp : public DyadicOp {
public:
  AplCodegen::RValue codegen(AplCodegen::LlvmCodegen *codegenManager,
                             AplCodegen::RValue lhs,
                             AplCodegen::RValue rhs) override;
  const string print() const override;
};

class ReshapeOp : public DyadicOp {
public:
  AplCodegen::RValue codegen(AplCodegen::LlvmCodegen *codegenManager,
                             AplCodegen::RValue lhs,
                             AplCodegen::RValue rhs) override;
  const string print() const override;
};
// endregion DyadicOp

// region HelperMethods
unique_ptr<DyadicOp> createDyadicOp(Symbol op);
unique_ptr<MonadicOp> createMonadicOp(Symbol op);
// endregion HelperMethods
} // namespace AplOp