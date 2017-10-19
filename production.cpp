#include "parser.h"
#include <iostream>

using namespace std;

Production::~Production() {
  while (!infixes.empty()) {
    PrInfixWS* p = infixes.front();
    if (p)
      delete(p);
    infixes.pop_front();
  }
}

string Production::to_string() {
  return "?";
}

void Production::flattenPostfixes() {
  //TODO: rewrite with iterators
  for (int i=infixes.size()-postfix_n;i<infixes.size();i++) {
    // flatten postfixes
    auto ws = infixes[i];
    if (ws) {
      ws->postfix_n = 0;
      // pull out nested infixes
      while (!ws->infixes.empty()) {
        auto nested_ws = ws->infixes.back();
        ws->infixes.pop_back();
        infixes.insert(infixes.begin() + i+1,nested_ws);
        postfix_n++;
      }
    }
  } 
}

PrDecor::PrDecor(Token t): rawToken(t) {}

string PrDecor::to_string() {
  return rawToken.value;
}

string PrExprParen::to_string() {
  return "(" + content->to_string() + ")";
}

PrExprArithmetic::PrExprArithmetic(PrExpression* lhs, Token op, PrExpression* rhs): lhs(lhs), op(op), rhs(rhs) {}

string PrExprArithmetic::to_string() {
  return ((lhs)?(lhs->to_string() + " "):"") + op.value + ((rhs)?(" " + rhs->to_string()):"");
}

PrAssignment::PrAssignment(PrExpression* lhs, Token op, PrExpression* rhs): lhs(lhs), op(op), rhs(rhs) {}

string PrAssignment::to_string() {
  if (op.type == OPR) {
    return lhs->to_string() + op.value;
  } else {
    return lhs->to_string() + " " + op.value + " " + rhs->to_string();
  }
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

string PrStatementFn::to_string() {
  return "@" + fn->to_string();
}

string PrBody::to_string() {
  string str = "{\n";
  for (auto p: productions) {
    str += p->to_string() + "\n";
  }
  str += "}";
  return str;
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

PrFinal::PrFinal(Token t): final(t) {}

string PrFinal::to_string() {
  if (final.type == STR)
    return "\"" + final.value + "\"";
  return "%" + final.value;
}

PrIdentifier::PrIdentifier(Token t): identifier(t) {}

string PrIdentifier::to_string() {
  return "$" + identifier.value;
}


PrVarDeclaration::PrVarDeclaration(Token t, PrExpression* expr): identifier(t), definition(expr) {}

string PrVarDeclaration::to_string() {
  if (definition != 0)
    return identifier.value + " = " + definition->to_string();
  return identifier.value;
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

string PrStatementIf::to_string() {
  string s = "if " + condition->to_string();
  s += "\n> " + result->to_string();
  if (otherwise)
    s += "\nelse\n> " + otherwise->to_string();
  return s;
}

string PrControl::to_string() {
  string s = kw.value;
  if (val)
    s += val->to_string();
  return s;
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

string PrWhile::to_string() {
  return "while " + condition->to_string() + "\n" + event->to_string();
}

string PrWith::to_string() {
  return "with " + objid->to_string() + "\n" + event->to_string();
}

string PrAccessorExpression::to_string() {
  string s = ds->to_string() + "[" + acc;
  for (auto index: indices) {
    s += index->to_string() + ", ";
  }
  return s.substr(0,s.length()-2) + "]";
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

string PrSwitch::to_string() {
  string s = "switch " + condition->to_string() + " {\n";
  
  for (auto c: cases) {
    s += c->to_string();
  }
  
  s += "}";
  return s;
}

PrInfixWS::PrInfixWS(Token t): val(t) {}

string PrInfixWS::to_string() {
  return val.value;
}
