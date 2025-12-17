// This abstract syntax tree draw inspiration from the Kaleidoscope tutorial
// from the official LLVM repository and the APL2C assignment from the DSLs for
// HPC (CS 8803 DSL) course at GeorgiaTech.
// https://github.com/llvm/llvm-project/blob/main/llvm/examples/Kaleidoscope/Chapter9/toy.cpp
// https://github.com/DSLs-for-HPC/APL2C

#include <iostream>
#include <string>
#include <vector>

namespace AplAst {
// Abstract Class for nodes in the APL AST
class Node {
public:
  virtual const std::string print() const;
};

// Literal node in the APL AST
class Literal : public Node {
  const std::vector<double> val;

public:
  Literal(const std::vector<double> val) : val(val) {}
  const std::vector<double> &getVal() const;
  static Literal *create(double val);
  static Literal *create(std::vector<double> old_vec, double new_elem);
  const std::string print() const override;
};

// Variable node in the APL AST
class Variable : public Node {
  std::string name;

public:
  Variable(const std::string &name) : name(name) {}
  const std::string &getName() const { return name; }
};

// Monadic Expression in the APL AST
class MonadicExpr : public Node {
  char opcode;
  std::unique_ptr<Node> operand;

public:
  MonadicExpr(char opcode, std::unique_ptr<Node> operand)
      : opcode(opcode), operand(std::move(operand)) {}
};

// Dyadic Expression in the APL AST
class DyadicExpr : public Node {
  char opcode;
  std::unique_ptr<Node> lhsOperand, rhsOperand;

public:
  DyadicExpr(char opcode, std::unique_ptr<Node> lhsOperand,
             std::unique_ptr<Node> rhsOperand)
      : opcode(opcode), lhsOperand(std::move(lhsOperand)),
        rhsOperand(std::move(rhsOperand)) {}
};

// Calls in the APL AST
class CallExpr : public Node {
  std::string callee;
  std::vector<std::unique_ptr<Node>> args;

public:
  CallExpr(const std::string &callee, std::vector<std::unique_ptr<Node>> args)
      : callee(callee), args(std::move(args)) {}
};

// Assignment statements in the APL AST
class AssignStmt : public Node {
  std::unique_ptr<Node> lhs, rhs;

public:
  AssignStmt(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs)
      : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};

// ostream overlead for APL AST Node
std::ostream &operator<<(std::ostream &os, const Node &node);
} // namespace AplAst