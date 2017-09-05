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
  return ((lhs)?(lhs->to_string() + " "):"") + op.value + ((rhs)?(" " + rhs->to_string()):"");
}

PrType PrExprArithmetic::get_type() {
  return PROD;
}

PrAssignment::PrAssignment(PrExpression* lhs, Token op, PrExpression* rhs): lhs(lhs), op(op), rhs(rhs) {}

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
  if (args.size() > 0)
    str = str.substr(0, str.size() - 1);
  str +=  ")";
  return str;
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
  return str;
}

PrType PrBody::get_type() {
  return PROD;
}

PrEmptyStatement::PrEmptyStatement(): enx(Token(ENX,"\n")), hastoken(false) {}

PrEmptyStatement::PrEmptyStatement(Token t): enx(t), hastoken(true) {}

string PrEmptyStatement::to_string() {
  if (!hastoken)
    return "";
  if (enx.value == ";")
    return "`";
  return "";
}

PrType PrEmptyStatement::get_type() {
  return PROD;
}

PrFinal::PrFinal(Token t): final(t) {}

string PrFinal::to_string() {
  if (final.type == STR)
    return "\"" + final.value + "\"";
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

PrVarDeclaration::PrVarDeclaration(Token t, PrExpression* expr): identifier(t), definition(expr) {}

string PrVarDeclaration::to_string() {
  if (definition != 0)
    return identifier.value + " = " + definition->to_string();
  return identifier.value;
}

PrType PrVarDeclaration::get_type() {
  return PROD;
}

string PrStatementVar::to_string() {
  string s = "var ";
  for (auto v: declarations) {
    s += v->to_string();
    s += ",";
  }
  s = s.substr(0,s.length() - 1);
  return s;
}

PrType PrStatementVar::get_type() {
  return PROD;
}

string PrStatementIf::to_string() {
  string s = "if " + condition->to_string();
  s += "\n> " + result->to_string();
  if (otherwise)
    s += "\nelse\n> " + otherwise->to_string();
  return s;
}

PrType PrStatementIf::get_type() {
  return PROD;
}

string PrControl::to_string() {
  string s = kw.value;
  if (val)
    s += val->to_string();
  return s;
}

PrType PrControl::get_type() {
  return PROD;
}

PrControl::PrControl(Token t, PrExpression* val): kw(t), val(val) {}

string PrFor::to_string() {
  string s = "for (";
  s += init->to_string();
  s += "; ";
  if (condition)
    s += condition->to_string();
  s += "; ";
  s += second->to_string();
  s += ")\n";
  s += "> " + first->to_string();
  return s;
}

PrType PrFor::get_type() {
  return PROD;
}

string PrWhile::to_string() {
  return "while " + condition->to_string() + "\n" + event->to_string();
}

PrType PrWhile::get_type() {
  return PROD;
}

string PrWith::to_string() {
  return "with " + objid->to_string() + "\n" + event->to_string();
}

PrType PrWith::get_type() {
  return PROD;
}

string PrAccessorExpression::to_string() {
  string s = ds->to_string() + "[" + acc;
  for (auto index: indices) {
    s += index->to_string() + ", ";
  }
  return s.substr(0,s.length()-2) + "]";
}

PrType PrAccessorExpression::get_type() {
  return PROD;
}

string PrCase::to_string() {
  string s;
  if (value)
    s = "case " + value->to_string() + ":\n";
  else
    s = "default:\n";
  
  for (auto p: productions)
    s += p->to_string() + "\n";
    
  return s;
}

PrType PrCase::get_type() {
  return PROD;
}

string PrSwitch::to_string() {
  string s = "switch " + condition->to_string() + " {\n";
  
  for (auto c: cases) {
    s += c->to_string();
  }
  
  s += "}";
  return s;
}

PrType PrSwitch::get_type() {
  return PROD;
}
