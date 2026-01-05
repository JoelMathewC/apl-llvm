#pragma once

#include <string>

using namespace std;

namespace AplOp {
enum Symbol { PLUS, MINUS, TIMES, DIVIDE, IOTA, RHO };

class Op {
public:
  virtual const string print() const;
};

// region MonadicOp
class MonadicOp : public Op {};

class NegateOp : public MonadicOp {
public:
  const string print() const override;
};

class IndexGenOp : public MonadicOp {
public:
  const string print() const override;
};
// endregion MonadicOp

// region DyadicOp
class DyadicOp : public Op {};

class AddOp : public DyadicOp {
public:
  const string print() const override;
};

class SubOp : public DyadicOp {
public:
  const string print() const override;
};

class MulOp : public DyadicOp {
public:
  const string print() const override;
};

class DivOp : public DyadicOp {
public:
  const string print() const override;
};

class ReshapeOp : public DyadicOp {
public:
  const string print() const override;
};
// endregion DyadicOp

// region HelperMethods
unique_ptr<DyadicOp> createDyadicOp(Symbol op);
unique_ptr<MonadicOp> createMonadicOp(Symbol op);
// endregion HelperMethods
} // namespace AplOp