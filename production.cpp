#include "parser.h"
#include <iostream>

using namespace std;

string Production::to_string() {
  return "?";
}

PrType Production::get_type() {
  return PROD;
}

PrDecor::PrDecor(Token t): rawToken(t) {}

string PrDecor::to_string() {
  return rawToken.value;
}

PrType PrDecor::get_type() {
  return PROD;
}

string PrExprParen::to_string() {
  return "(" + content->to_string() + ")";
}

PrType PrExprParen::get_type() {
  return PROD;
}

PrExprArithmetic::PrExprArithmetic(PrExpression* lhs, Token op, PrExpression* rhs): lhs(lhs), op(op), rhs(rhs) {}

string PrExprArithmetic::to_string() {
  return lhs->to_string() + " " + op.value + " " + rhs->to_string();
}

PrType PrExprArithmetic::get_type() {
  return PROD;
}

PrAssignable::PrAssignable(Token t): identifier(t) {}

string PrAssignable::to_string() {
  return "$<" + identifier.value + ">";
}

PrType PrAssignable::get_type() {
  return PROD;
}

PrAssignment::PrAssignment(PrAssignable* lhs, Token op, PrExpression* rhs): lhs(lhs), op(op), rhs(rhs) {}

string PrAssignment::to_string() {
  if (op.type == OPR) {
    return lhs->to_string() + op.value;
  } else {
    return lhs->to_string() + " " + op.value + " " + rhs->to_string();
  }
}

PrType PrAssignment::get_type() {
  return PROD;
}

PrExpressionFn::PrExpressionFn(Token id): identifier(id) {}

string PrExpressionFn::to_string() {
  string str = identifier.value + "(";
  for (auto arg: args) {
    str += arg->to_string() + ",";
  }
  str = str.substr(0, str.size() - 1) + ")";
}

PrType PrExpressionFn::get_type() {
  return PROD;
}

string PrStatementFn::to_string() {
  return "@" + fn->to_string();
}

PrType PrStatementFn::get_type() {
  return PROD;
}

string PrBody::to_string() {
  string str = "{\n";
  for (auto p: productions) {
    str += p->to_string() + "\n";
  }
  str += "}";
}

PrType PrBody::get_type() {
  return PROD;
}

PrEmptyStatement::PrEmptyStatement(Token t): enx(t) {}

string PrEmptyStatement::to_string() {
  if (enx.value == ";")
    return "`";
  return "";
}

PrType PrEmptyStatement::get_type() {
  return PROD;
}

PrFinal::PrFinal(Token t): final(t) {}

string PrFinal::to_string() {
  return "%" + final.value;
}

PrType PrFinal::get_type() {
  return PROD;
}

PrIdentifier::PrIdentifier(Token t): identifier(t) {}

string PrIdentifier::to_string() {
  return "$" + identifier.value;
}

PrType PrIdentifier::get_type() {
  return PROD;
}
