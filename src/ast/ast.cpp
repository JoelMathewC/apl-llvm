#include "ast.hpp"
#include <iostream>
#include <vector>

namespace AplAst {
const std::string Node::print() const { return "LITERAL"; }
const std::vector<double> &Literal::getVal() const { return this -> val; }

Literal *Literal::create(double val) {
  std::vector<double> vec = {val};
  return new Literal(vec);
}

Literal *Literal::create(std::vector<double> old_vec, double new_elem) {
  old_vec.push_back(new_elem);
  return new Literal(old_vec);
}

std::ostream &operator<<(std::ostream &os, const AplAst::Node &node) {
  return os << node.print();
}
} // namespace AplAst