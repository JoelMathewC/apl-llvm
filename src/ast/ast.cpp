#include "ast.hpp"
#include <iostream>
#include <vector>

using namespace std;

namespace AplAst {
const string Term::print() const { return "TERM"; }

ostream &operator<<(ostream &os, const Term &term) {
  return os << term.print();
}

// #pragma region Literal
unique_ptr<Literal> Literal::create(double val) {
  vector<double> vec = {val};
  return std::move(make_unique<Literal>(vec));
}

unique_ptr<Literal> Literal::create(vector<double> old_vec, double new_elem) {
  old_vec.push_back(new_elem);
  return std::move(make_unique<Literal>(old_vec));
}

const vector<double> &Literal::getVal() const { return this->val; }
const string Literal::print() const { return "LITERAL"; }
// #pragma endregion Literal

// #pragma region Variable
unique_ptr<Variable> Variable::create(const string &name) {
  return std::move(make_unique<Variable>(name));
}

const string &Variable::getName() const { return name; }

const string Variable::print() const { return "VARIABLE{" + this->name + "}"; }
// #pragma endregion Variable

// #pragma region Call
unique_ptr<Call> Call::create(const Operator op, unique_ptr<Node> &arg) {
  vector<unique_ptr<Node>> vec;
  vec.push_back(std::move(arg));
  return std::move(make_unique<Call>(op, vec));
}

unique_ptr<Call> Call::create(const Operator op, unique_ptr<Node> &arg1,
                              unique_ptr<Node> &arg2) {
  vector<unique_ptr<Node>> vec;
  vec.push_back(std::move(arg1));
  vec.push_back(std::move(arg2));
  return std::move(make_unique<Call>(op, vec));
}
const Operator &Call::getOp() const { return this->op; }
const vector<unique_ptr<Node>> &Call::getArgs() const { return this->args; };
const string Call::print() const {
  // generate comma seperated string for arguments
  string args_str = "";
  for (auto &arg : this->args) {
    if (args_str.size() > 0)
      args_str += ',';
    args_str += arg->print();
  }

  // TODO: cast this->op to appropriate string
  return "Call(" + string(1, this->op) + string(1, ',') + args_str + ")";
}
// #pragma endregion Call

// #pragma region AssignStmt
unique_ptr<AssignStmt> AssignStmt::create(unique_ptr<Variable> &lhs,
                                          unique_ptr<Node> &rhs) {
  return std::move(make_unique<AssignStmt>(lhs, rhs));
}

const unique_ptr<Variable> &AssignStmt::getLhs() const { return this->lhs; }
const unique_ptr<Node> &AssignStmt::getRhs() const { return this->rhs; }
const string AssignStmt::print() const {
  return "Assign[" + this->lhs->print() + " = " + this->rhs->print() + "]";
}
// #pragma endregion AssignStmt
} // namespace AplAst