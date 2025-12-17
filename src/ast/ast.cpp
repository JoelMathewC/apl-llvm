#include "ast.hpp"
#include <iostream>

std::ostream &operator<<(std::ostream &os, const AplAst::Literal &literal) {
  return os << "Literal";
}