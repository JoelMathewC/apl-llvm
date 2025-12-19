#include "ast.hpp"
#include <iostream>
#include <map>
#include <vector>

using namespace std;

namespace AplAst {
map<Operator, string> operatorToStringMapping = {
    {Operator::ADD, "add"}, {Operator::SUB, "sub"}, {Operator::MUL, "mul"},
    {Operator::DIV, "div"}, {Operator::EXP, "exp"},
};

const string Term::print() const { return "TERM"; }

ostream &operator<<(ostream &os, const Term &term) {
  return os << term.print();
}

// Literal section
Literal::Literal(const vector<double> val) : val(val) {}

unique_ptr<Literal> Literal::create(double val) {
  vector<double> vec = {val};
  return make_unique<Literal>(vec);
}

unique_ptr<Literal> Literal::create(vector<double> vec, double new_elem) {
  vec.push_back(new_elem);
  return make_unique<Literal>(vec);
}

const vector<double> &Literal::getVal() const { return this->val; }
const string Literal::print() const { return "LITERAL"; }
// end Literal section

// Variable section
Variable::Variable(const string &name) : name(name) {}

unique_ptr<Variable> Variable::create(const string &name) {
  return make_unique<Variable>(name);
}

const string &Variable::getName() const { return name; }

const string Variable::print() const { return "VARIABLE(" + this->name + ")"; }
// end Variable section

// Call section
Call::Call(const Operator op, vector<unique_ptr<Node>> &args)
    : op(op), args(std::move(args)) {}

unique_ptr<Call> Call::create(const Operator op, unique_ptr<Node> &arg) {
  vector<unique_ptr<Node>> vec;
  vec.push_back(std::move(arg));
  return make_unique<Call>(op, vec);
}

unique_ptr<Call> Call::create(const Operator op, unique_ptr<Node> &arg1,
                              unique_ptr<Node> &arg2) {
  vector<unique_ptr<Node>> vec;
  vec.push_back(std::move(arg1));
  vec.push_back(std::move(arg2));
  return make_unique<Call>(op, vec);
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

  return "Call(" + operatorToStringMapping[this->op] + string(1, ',') +
         args_str + ")";
}
// end Call section

// Assign section
AssignStmt::AssignStmt(const string &varName, unique_ptr<Node> &rhs)
    : lhs(make_unique<Variable>(varName)), rhs(std::move(rhs)) {}

unique_ptr<AssignStmt> AssignStmt::create(const string &varName,
                                          unique_ptr<Node> &rhs) {
  return make_unique<AssignStmt>(varName, rhs);
}

const unique_ptr<Variable> &AssignStmt::getLhs() const { return this->lhs; }

const unique_ptr<Node> &AssignStmt::getRhs() const { return this->rhs; }

const string AssignStmt::print() const {
  return "Assign(" + this->lhs->print() + "," + this->rhs->print() + ")";
}
// end Assign section
} // namespace AplAst