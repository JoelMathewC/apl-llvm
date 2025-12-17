/*
    This abstract syntax tree draw inspiration from the Kaleidoscope tutorial
   from the official LLVM repository and the APL2C assignment from the DSLs for
   HPC (CS 8803 DSL) course at GeorgiaTech.

   https://github.com/llvm/llvm-project/blob/main/llvm/examples/Kaleidoscope/Chapter9/toy.cpp
   https://github.com/DSLs-for-HPC/APL2C
*/

#include <string>
#include <vector>

namespace AplAst {
class Node {};

class Variable : Node {
  std::string name;

public:
  Variable(const std::string &name) : name(name) {}
  const std::string &getName() const { return name; }
};

class Literal : Node {
  const std::vector<double> val;

public:
  Literal(const std::vector<double> val) : val(val) {}
  const std::vector<double> &getVal() const { return val; }
  static Literal *create(double val) {
    std::vector<double> vec = {val};
    return new AplAst::Literal(vec);
  }
  static Literal *create(std::vector<double> old_vec, double new_elem) {
    old_vec.push_back(new_elem);
    return new AplAst::Literal(old_vec);
  }
};

class MonadicExpr : Node {
  char opcode;
  std::unique_ptr<Node> operand;

public:
  MonadicExpr(char opcode, std::unique_ptr<Node> operand)
      : opcode(opcode), operand(std::move(operand)) {}
};

class DyadicExpr : Node {
  char opcode;
  std::unique_ptr<Node> lhsOperand, rhsOperand;

public:
  DyadicExpr(char opcode, std::unique_ptr<Node> lhsOperand,
             std::unique_ptr<Node> rhsOperand)
      : opcode(opcode), lhsOperand(std::move(lhsOperand)),
        rhsOperand(std::move(rhsOperand)) {}
};

class CallExpr : Node {
  std::string callee;
  std::vector<std::unique_ptr<Node>> args;

public:
  CallExpr(const std::string &callee, std::vector<std::unique_ptr<Node>> args)
      : callee(callee), args(std::move(args)) {}
};

class AssignStmt : Node {
  std::unique_ptr<Node> lhs, rhs;

public:
  AssignStmt(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs)
      : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};
} // namespace AplAst