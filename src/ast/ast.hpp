// This abstract syntax tree draw inspiration from the Kaleidoscope tutorial
// from the official LLVM repository and the APL2C assignment from the DSLs for
// HPC (CS 8803 DSL) course at GeorgiaTech.
// https://github.com/llvm/llvm-project/blob/main/llvm/examples/Kaleidoscope/Chapter9/toy.cpp
// https://github.com/DSLs-for-HPC/APL2C
#pragma once

#include "op.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace AplAst {
// Root base class for APL AST
class Term {
public:
  virtual const string print() const;
};

// Abstract class for nodes in APL AST that evaluate to expressions
class Node : public Term {};

// Abstract class for nodes in APL AST that are standalone statements
class Tree : public Term {};

// Literal node in the APL AST
class Literal : public Node {
  const vector<double> val;

public:
  Literal(const vector<double> val);
  static unique_ptr<Literal> create(double val);
  static unique_ptr<Literal> create(vector<double> vec, double new_elem);
  const vector<double> &getVal() const;
  const string print() const override;
};

// Variable node in the APL AST
class Variable : public Node {
  const string name;

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

public:
  Call(char op, vector<unique_ptr<Node>> &args);
  static unique_ptr<Call> create(char op, unique_ptr<Node> &arg);
  static unique_ptr<Call> create(char op, unique_ptr<Node> &arg1,
                                 unique_ptr<Node> &arg2);
  const unique_ptr<AplOp::Op> &getOp() const;
  const vector<unique_ptr<Node>> &getArgs() const;
  const string print() const override;
};

// Assignment statements in the APL AST
class AssignStmt : public Tree {
  const unique_ptr<Variable> lhs;
  const unique_ptr<Node> rhs;

public:
  AssignStmt(const string &varName, unique_ptr<Node> &rhs);
  static unique_ptr<AssignStmt> create(const string &varName,
                                       unique_ptr<Node> &rhs);
  const unique_ptr<Variable> &getLhs() const;
  const unique_ptr<Node> &getRhs() const;
  const string print() const override;
};

// ostream overlead for APL AST Node
ostream &operator<<(ostream &os, const Term &term);
} // namespace AplAst