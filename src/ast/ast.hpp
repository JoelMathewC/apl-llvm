// This abstract syntax tree draw inspiration from the Kaleidoscope tutorial
// from the official LLVM repository and the APL2C assignment from the DSLs for
// HPC (CS 8803 DSL) course at GeorgiaTech.
// https://github.com/llvm/llvm-project/blob/main/llvm/examples/Kaleidoscope/Chapter9/toy.cpp
// https://github.com/DSLs-for-HPC/APL2C

#include <iostream>
#include <string>
#include <vector>

namespace AplAst {
enum Operator { ADD, SUB, MUL, DIV, EXP };

// Root base class for APL AST
class Term {
public:
  virtual const std::string print() const;
};

// Abstract class for nodes in APL AST that evaluate to expressions
class Node : public Term {};

// Abstract class for nodes in APL AST that are standalone statements
class Tree : public Term {};

// Literal node in the APL AST
class Literal : public Node {
  const std::vector<double> val;

public:
  Literal(const std::vector<double> val) : val(val) {}
  static Literal *create(double val);
  static Literal *create(std::vector<double> old_vec, double new_elem);
  const std::vector<double> &getVal() const;
  const std::string print() const override;
};

// Variable node in the APL AST
class Variable : public Node {
  const std::string name;

public:
  Variable(const std::string &name) : name(name) {}
  const std::string &getName() const;
  const std::string print() const override;
};

// An APL AST expression node that evaluates op on args
class Call : public Node {
  const Operator op;
  const std::vector<std::unique_ptr<Node>> args;

public:
  Call(const Operator op, std::vector<std::unique_ptr<Node>> args)
      : op(op), args(std::move(args)) {}
  static Call *create(const Operator op, std::unique_ptr<Node> arg);
  static Call *create(const Operator op, std::unique_ptr<Node> arg1,
                      std::unique_ptr<Node> arg2);
  const Operator &getOp() const;
  const std::vector<std::unique_ptr<Node>> &getArgs() const;
  const std::string print() const override;
};

// Assignment statements in the APL AST
class AssignStmt : public Tree {
  const std::unique_ptr<Variable> lhs;
  const std::unique_ptr<Node> rhs;

public:
  AssignStmt(std::unique_ptr<Variable> lhs, std::unique_ptr<Node> rhs)
      : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
  const std::unique_ptr<Variable> &getLhs() const;
  const std::unique_ptr<Node> &getRhs() const;
  const std::string print() const override;
};

// ostream overlead for APL AST Node
std::ostream &operator<<(std::ostream &os, const Term &term);
} // namespace AplAst