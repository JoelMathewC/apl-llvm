#include "ast.hpp"
#include <iostream>
#include <vector>

namespace AplAst {
const std::string Term::print() const { return "TERM"; }

std::ostream &operator<<(std::ostream &os, const Node &node) {
  return os << node.print();
}

#pragma region Literal
Literal *Literal::create(double val) {
  std::vector<double> vec = {val};
  return new Literal(vec);
}

Literal *Literal::create(std::vector<double> old_vec, double new_elem) {
  old_vec.push_back(new_elem);
  return new Literal(old_vec);
}

const std::vector<double> &Literal::getVal() const { return this->val; }
const std::string Literal::print() const { return "LITERAL"; }
#pragma endregion Literal

#pragma region Variable
const std::string &Variable::getName() const { return name; }

const std::string Variable::print() const {
  return "VARIABLE{" + this->name + "}";
}
#pragma endregion Variable

#pragma region Call
Call *Call::create(const Operator op, std::unique_ptr<Node> arg) {
  std::vector<std::unique_ptr<Node>> vec = {std::move(arg)};
  return new Call(op, vec);
}

Call *Call::create(const Operator op, std::unique_ptr<Node> arg1,
                           std::unique_ptr<Node> arg2) {
  std::vector<std::unique_ptr<Node>> vec = {std::move(arg1), std::move(arg2)};
  return new Call(op, vec);
}
const Operator &Call::getOp() const { return this->op; }
const std::vector<std::unique_ptr<Node>> &Call::getArgs() const {
  return this->args;
};
const std::string Call::print() const {
  // generate comma seperated string for arguments
  std::string args_str = "";
  for (int i = 0; i < this->args.size(); ++i) {
    if (args_str.size() > 0)
      args_str += ',';
    args_str += this->args[i]->print();
  }
  return "Call(" + this->op + ',' + args_str + ")";
}
#pragma endregion Call

#pragma region AssignStmt
const std::unique_ptr<Variable> &AssignStmt::getLhs() const {
  return this->lhs;
}
const std::unique_ptr<Node> &AssignStmt::getRhs() const { return this->rhs; }
const std::string AssignStmt::print() const {
  return "Assign[" + this->lhs->print() + " = " + this->rhs->print() + "]";
}
#pragma endregion AssignStmt
} // namespace AplAst