#include "ast.hpp"
#include "op.hpp"
#include <iostream>
#include <map>
#include <vector>

#include "../codegen/codegen.hpp"
#include "llvm/IR/IRBuilder.h"

using namespace std;

namespace AplAst {
// LocalFunctions section
unique_ptr<AplOp::Op> createOp(char op) {
  // TODO: add support for ÷
  switch (op) {
  case '+':
    return make_unique<AplOp::AddOp>();
  case '-':
    return make_unique<AplOp::SubOp>();
  case 'x':
    return make_unique<AplOp::MulOp>();
  case '*':
    return make_unique<AplOp::ExpOp>();
  default:
    return make_unique<AplOp::Op>();
  }
}
// End LocalFunctions section

// Term section
Term::~Term() = default;

const string Term::print() const { return "TERM"; }

llvm::Value *Term::codegen_(AplCodegen::LlvmCodegen *codegenManager) {
  return nullptr;
}

llvm::Value *Term::codegen(AplCodegen::LlvmCodegen *codegenManager,
                           bool isTopLvlExpr) {
  return nullptr;
}
// End Term section

// Node section
const vector<unsigned long> Node::getShape() { return this->shape; }

Node::Node(const vector<unsigned long> shape) : shape(shape) {}

llvm::Value *Node::codegen(AplCodegen::LlvmCodegen *codegenManager,
                           bool isTopLvlExpr) {
  if (isTopLvlExpr)
    return codegenManager->wrapInAnonymousFunction(
        this->codegen_(codegenManager));
  return this->codegen_(codegenManager);
}
// End Node Section

// Tree section
llvm::Value *Tree::codegen(AplCodegen::LlvmCodegen *codegenManager,
                           bool isTopLvlExpr) {
  return this->codegen_(codegenManager);
}
// End Tree section

// Literal section
Literal::Literal(const vector<float> val)
    : val(val), Node(vector<unsigned long>{val.size()}) {}

unique_ptr<Literal> Literal::create(float val) {
  vector<float> vec = {val};
  return make_unique<Literal>(vec);
}

unique_ptr<Literal> Literal::create(vector<float> vec, float new_elem) {
  vec.push_back(new_elem);
  return make_unique<Literal>(vec);
}

const vector<float> &Literal::getVal() const { return this->val; }

llvm::Value *Literal::codegen_(AplCodegen::LlvmCodegen *codegenManager) {
  return codegenManager->literalCodegen(this->val);
}

const string Literal::print() const {
  // generate comma seperated string of array
  string vec_str = "";
  for (auto &elem : this->val) {
    vec_str += to_string(elem);
    vec_str += ",";
  }

  // remove the trailing comma
  vec_str.pop_back();

  return "LITERAL(" + vec_str + ")";
}
// end Literal section

// Variable section
Variable::Variable(const string &name)
    : name(name), Node(vector<unsigned long>{0}) {}

unique_ptr<Variable> Variable::create(const string &name) {
  return make_unique<Variable>(name);
}

const string &Variable::getName() const { return name; }

llvm::Value *Variable::codegen_(AplCodegen::LlvmCodegen *codegenManager) {
  return codegenManager->variableCodegen(this->name);
}

const string Variable::print() const { return "VARIABLE(" + this->name + ")"; }
// end Variable section

// Call section
// TODO: remove the redundant createOp here and fix the result shape logic
Call::Call(char op, vector<unique_ptr<Node>> &args)
    : op(createOp(op)), args(std::move(args)),
      Node(createOp(op)->getResultShape(
          vector<vector<unsigned long>>{args[0]->getShape()})) {}

unique_ptr<Call> Call::create(char op, unique_ptr<Node> &arg) {
  vector<unique_ptr<Node>> vec;
  vec.push_back(std::move(arg));
  return make_unique<Call>(op, vec);
}

unique_ptr<Call> Call::create(char op, unique_ptr<Node> &arg1,
                              unique_ptr<Node> &arg2) {
  vector<unique_ptr<Node>> vec;
  vec.push_back(std::move(arg1));
  vec.push_back(std::move(arg2));
  return make_unique<Call>(op, vec);
}

const unique_ptr<AplOp::Op> &Call::getOp() const { return this->op; }

const vector<unique_ptr<Node>> &Call::getArgs() const { return this->args; }

llvm::Value *Call::codegen_(AplCodegen::LlvmCodegen *codegenManager) {
  return codegenManager->addCodegen(
      this->args[0]->codegen(codegenManager, false),
      this->args[1]->codegen(codegenManager, false));
}

const string Call::print() const {
  // generate comma seperated string for arguments
  string args_str = "";
  for (auto &arg : this->args) {
    if (args_str.size() > 0)
      args_str += ',';
    args_str += arg->print();
  }

  return "Call(" + this->op->print() + string(1, ',') + args_str + ")";
}
// end Call section

// Assign section
AssignStmt::AssignStmt(const string varName, unique_ptr<Node> &rhs)
    : Node(rhs->getShape()), varName(varName), rhs(std::move(rhs)) {}

unique_ptr<AssignStmt> AssignStmt::create(const string varName,
                                          unique_ptr<Node> &rhs) {
  return make_unique<AssignStmt>(varName, rhs);
}

const string AssignStmt::getVarName() const { return this->varName; }

const unique_ptr<Node> &AssignStmt::getRhs() const { return this->rhs; }

llvm::Value *AssignStmt::codegen_(AplCodegen::LlvmCodegen *codegenManager) {
  return codegenManager->assignCodegen(this->varName, this->rhs->codegen(codegenManager, false));
}

const string AssignStmt::print() const {
  return "Assign(" + this->varName + "," + this->rhs->print() + ")";
}
// end Assign section

// Misc. section
ostream &operator<<(ostream &os, const Term &term) {
  return os << term.print();
}
// end Misc. section
} // namespace AplAst