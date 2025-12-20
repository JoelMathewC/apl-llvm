#include "op.hpp"
#include <string>

using namespace std;

namespace AplOp {
const string Op::print() const { return "unspecified-op"; }
const string AddOp::print() const { return "+"; }
const string SubOp::print() const { return "-"; }
const string MulOp::print() const { return "x"; }
const string ExpOp::print() const { return "*"; }
const string DivOp::print() const { return "÷"; }
} // namespace AplOp