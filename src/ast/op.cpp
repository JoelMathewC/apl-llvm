#include "op.hpp"
#include <string>
#include <vector>

using namespace std;

namespace AplOp {
const string Op::print() const { return "unspecified-op"; }
const vector<unsigned long>
Op::getResultShape(vector<vector<unsigned long>> argShape) const {
  return vector<unsigned long>{};
}

// TODO: add logic to actually test if both params have the same shape
const vector<unsigned long>
ShapeRetainingOp::getResultShape(vector<vector<unsigned long>> argShape) const {
  vector<unsigned long> res;
  for (int i = 0; i < argShape[0].size(); ++i) {
    res.push_back(argShape[0][i]);
  }
  return res;
}

const string AddOp::print() const { return "+"; }
const string SubOp::print() const { return "-"; }
const string MulOp::print() const { return "x"; }
const string ExpOp::print() const { return "*"; }
const string DivOp::print() const { return "÷"; }
} // namespace AplOp