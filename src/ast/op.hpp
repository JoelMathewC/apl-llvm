#pragma once

#include <string>
using namespace std;

// TODO: make all the classes here static if need be
namespace AplOp {
class Op {
public:
  virtual const vector<unsigned long>
  getResultShape(vector<vector<unsigned long>> argShape) const;
  virtual const string print() const;
};

class ShapeRetainingOp : public Op {
public:
  const vector<unsigned long>
  getResultShape(vector<vector<unsigned long>> argShape) const override;
};

class AddOp : public ShapeRetainingOp {
public:
  const string print() const override;
};

class SubOp : public ShapeRetainingOp {
public:
  const string print() const override;
};

class MulOp : public ShapeRetainingOp {
public:
  const string print() const override;
};

class DivOp : public ShapeRetainingOp {
public:
  const string print() const override;
};

class ExpOp : public ShapeRetainingOp {
public:
  const string print() const override;
};
} // namespace AplOp