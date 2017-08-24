#include "parser.h"
#include <iostream>

string Production::to_string() {
  return "?"
}

PrType Production::get_type() {
  return 0;
}

string PrDecor::to_string() {
  return rawToken->value;
}

PrType PrDecor::get_type() {
  return 0;
}

string PrExprParen::to_string() {
  return "(" + content->to_string() + ")";
}

PrType PrExprParen::get_type() {
  return 0;
}

string PrExprArithmetic::to_string() {
  return return lhs->to_string() + " " + op->value + " " + rhs->to_string();
}

PrType PrExprArithmetic::get_type() {
  return 0;
}


string PrAssignable::to_string() {
  return "$<" + identifier->value + ">"
}

PrType PrAssignable::get_type() {
  return 0;
}

string PrAssignment::to_string() {
  if (op.type == OPR) {
    return lhs->to_string() + op->value;
  } else {
    return lhs->to_string() + " " + op->value + " " + rhs->to_string();
  }
}

PrType PrAssignment::get_type() {
  return 0;
}

struct PrExpressionFn: PrExpression{
  virtual string to_string();
  virtual PrType get_type();
  
  Token fn;
  vector<PrExpression*> args;
}

string PrExpressionFn::to_string() {
  string str = fn->value + "(";
  for (auto arg: args) {
    str += arg->to_string() + ",";
  }
  str = str.substr(0, str.size() - 1) + ")";
}

PrType PrExpressionFn::get_type() {
  return 0;
}

string PrExpressionFn::to_string() {
  return "@" + fn->to_string();
}

PrType PrExpressionFn::get_type() {
  return 0;
}

string PrBody::to_string() {
  string str = "{\n"
  for (auto p: productions) {
    str += p->to_string() + "\n"
  }
  str += "}"
}

PrType PrBody::get_type() {
  return 0;
}

