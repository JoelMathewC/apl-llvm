// This abstract syntax tree draw inspiration from the Kaleidoscope tutorial
// from the official LLVM repository and the APL2C assignment from the DSLs for
// HPC (CS 8803 DSL) course at GeorgiaTech.
// https://github.com/llvm/llvm-project/blob/main/llvm/examples/Kaleidoscope/Chapter9/toy.cpp
// https://github.com/DSLs-for-HPC/APL2C
#pragma once

#include "../codegen/codegen.hpp"
#include "op.hpp"
#include "llvm/IR/IRBuilder.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace AplAst {
// Abstract class for nodes in APL AST that evaluate to expressions
class Node {
protected:
  const vector<unsigned long> shape;
  Node(const vector<unsigned long> shape);

public:
  virtual llvm::Value *codegen(AplCodegen::LlvmCodegen *codegenManager);
  const vector<unsigned long> getShape();
  virtual const string print() const;
  virtual ~Node();
};

// Literal node in the APL AST
class Literal : public Node {
  const vector<float> val;

public:
  Literal(const vector<float> val);
  static unique_ptr<Literal> create(float val);
  static unique_ptr<Literal> create(vector<float> vec, float new_elem);
  const vector<float> &getVal() const;
  llvm::Value *codegen(AplCodegen::LlvmCodegen *codegenManager) override;
  const string print() const override;
};

// An APL AST expression node that evaluates dyadic ops on args
class DyadicCall : public Node {
  const unique_ptr<AplOp::DyadicOp> op;
  const unique_ptr<Node> arg1;
  const unique_ptr<Node> arg2;

public:
  DyadicCall(unique_ptr<AplOp::DyadicOp> op, unique_ptr<Node> arg1,
             unique_ptr<Node> arg2);
  static unique_ptr<DyadicCall> create(char op, unique_ptr<Node> &arg1,
                                       unique_ptr<Node> &arg2);
  llvm::Value *codegen(AplCodegen::LlvmCodegen *codegenManager) override;
  const string print() const override;
};

// ostream overlead for APL AST Node
ostream &operator<<(ostream &os, const Node &node);
} // namespace AplAst