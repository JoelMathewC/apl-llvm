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
  virtual llvm::Value *codegen_(AplCodegen::LlvmCodegen *codegenManager);
  const vector<unsigned long> shape;
  Node(const vector<unsigned long> shape);

public:
  const vector<unsigned long> getShape();
  llvm::Value *codegen(AplCodegen::LlvmCodegen *codegenManager,
                       bool isTopLvlExpr);
  virtual const string print() const;
  virtual ~Node();
};

// Literal node in the APL AST
class Literal : public Node {
  const vector<float> val;
  llvm::Value *codegen_(AplCodegen::LlvmCodegen *codegenManager) override;

public:
  Literal(const vector<float> val);
  static unique_ptr<Literal> create(float val);
  static unique_ptr<Literal> create(vector<float> vec, float new_elem);
  const vector<float> &getVal() const;
  const string print() const override;
};

// Variable node in the APL AST
class Variable : public Node {
  const string name;
  llvm::Value *codegen_(AplCodegen::LlvmCodegen *codegenManager) override;

public:
  Variable(const string &name);
  static unique_ptr<Variable> create(const string &name);
  const string &getName() const;
  const string print() const override;
};

// An APL AST expression node that evaluates op on args
class Call : public Node {
  const unique_ptr<AplOp::Op> op;
  const vector<unique_ptr<Node>> args;
  llvm::Value *codegen_(AplCodegen::LlvmCodegen *codegenManager) override;

public:
  Call(char op, vector<unique_ptr<Node>> &args);
  static unique_ptr<Call> create(char op, unique_ptr<Node> &arg);
  static unique_ptr<Call> create(char op, unique_ptr<Node> &arg1,
                                 unique_ptr<Node> &arg2);
  const unique_ptr<AplOp::Op> &getOp() const;
  const vector<unique_ptr<Node>> &getArgs() const;
  const string print() const override;
};

// ostream overlead for APL AST Node
ostream &operator<<(ostream &os, const Node &node);
} // namespace AplAst