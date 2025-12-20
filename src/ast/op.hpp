#pragma once

#include <string>
using namespace std;

namespace AplOp {
class Op {
public:
  virtual const string print() const;
};

class AddOp : public Op {
public:
  const string print() const override;
};

class SubOp : public Op {
public:
  const string print() const override;
};

class MulOp : public Op {
public:
  const string print() const override;
};

class DivOp : public Op {
public:
  const string print() const override;
};

class ExpOp : public Op {
public:
  const string print() const override;
};
} // namespace AplOp