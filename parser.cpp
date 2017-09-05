#include "parser.h"

using namespace std;

Parser::Parser(istream* is): ts(is, 4) {

}

Production* Parser::read() {
  if (ts.peek().type == END || ts.peek().type == ERR)
    return nullptr;
  return read_production();
}

Production* Parser::read_production() {
  TokenType t = ts.peek().type;
  if (t == COMMENT || t == WS)
    return read_rawtoken();
  PrStatement* p = read_statement();
  read_statement_end();
  return p;
}

PrDecor* Parser::read_rawtoken() {
  return new PrDecor(ts.read());
}

PrStatement* Parser::read_statement() {  
  string value(ts.peek().value);
  switch (ts.peek().type) {
  case ENX:
    return new PrEmptyStatement(ts.read());
  case KW:
    if (value == "var")
      return read_statement_var();
    if (value == "if")
      return read_statement_if();
    if (value == "for")
      return read_for();
    if (value == "return")
      return new PrControl(ts.read(),read_expression());
    if (value == "exit" || value == "continue" || value == "break")
      return new PrControl(ts.read());
    return nullptr;
  case PUNC:
    if (value == "(") {
      return read_assignment();
    }
    if (value == "{") {
      return read_block();
    }
    return nullptr;
  case ID:
    {
      Token t = ts.peek(1);
      if (t == Token(PUNC,"("))
        return read_statement_function();
      return read_assignment();
    }
  }
}

PrAssignment* Parser::read_assignment() {
  PrAssignable* lhs = read_assignable();
  Token op = ts.read();
  PrExpression* rhs = 0;
  if (op.type != OPR) {
    rhs = read_expression();
  }
  PrAssignment* a = new PrAssignment(lhs,op,rhs);
  
  return a;
}

PrAssignable* Parser::read_assignable() {
  Token t(ts.peek());
  PrAssignable* a;
  if (t == Token(PUNC,"(")) {
    // TODO
    return 0;
  } else {
    t = ts.read();
    a = new PrAssignable(t);
  }
  // optional: . or []
  
  return a;
}

PrExpression* Parser::read_expression() {
  PrExpression* to_return = 0;

  Token t(ts.peek());
  if (t == Token(OP,"-") || t.type == OPR)
    return new PrExprArithmetic(nullptr, ts.read(),read_expression());
  if (t.type == NUM || t.type == STR)
    to_return = new PrFinal(ts.read());
  if (t == Token(PUNC,"("))
    to_return = read_expression_parentheses();
  else if (t.type == ID) {
    t = ts.peek(1);
    if (t == Token(PUNC,"("))
      to_return = read_expression_function();
    else
      to_return = new PrIdentifier(ts.read());
  }
  t = ts.peek();
  if (t.type == OP)
    return read_arithmetic(to_return);
  else
    return to_return;
} 

PrExprArithmetic* Parser::read_arithmetic(PrExpression* lhs) {
  //TODO assert ts.peek() is operator
  if (ts.peek().type == OPR)
    return new PrExprArithmetic(lhs, ts.read(), nullptr);
  return new PrExprArithmetic(lhs, ts.read(), read_expression());
}

PrExpressionFn* Parser::read_expression_function() {
  PrExpressionFn* pfn = new PrExpressionFn(ts.read());
  
  // read paren:
  ts.read();  
  
  while (true) {
    Token next(ts.peek());
    if (next == Token(PUNC,")"))
      break;
    pfn->args.push_back(read_expression());
    next = ts.read();
    if (next == Token(PUNC,","))
      continue;
    else break;
  }
  
  // read paren
  ts.read();
  
  return pfn;
}

PrStatementFn* Parser::read_statement_function() {
  PrStatementFn* fn = new PrStatementFn();
  fn->fn = read_expression_function();
  return fn;
}

PrExprParen* Parser::read_expression_parentheses() {
  PrExprParen* p = new PrExprParen();
  ts.read(); //(
  p->content = read_expression();
  ts.read(); //)
  return p;
}

PrStatementVar* Parser::read_statement_var() {
  PrStatementVar* p = new PrStatementVar();
  ts.read(); // read "var"
  
  while (true) {
    // read var declaration:
    ignoreWS();
    PrVarDeclaration* d = new PrVarDeclaration(ts.read());
    ignoreWS();
    if (ts.peek() == Token(OP,"=")) {
      ts.read();
      ignoreWS();
      d->definition = read_expression();
    }
    p->declarations.push_back(d);
    ignoreWS();
    if (ts.peek() == Token(PUNC,",")) {
      ts.read();
      continue;
    }
    break;
  }
  
  return p;
}

PrStatementIf* Parser::read_statement_if() {
  ts.read(); // read IF
  ignoreWS();
  PrStatementIf* p = new PrStatementIf();
  ignoreWS();
  p->condition = read_expression();
  ignoreWS();
  p->result = read_statement();
  read_statement_end();
  ignoreWS();
  if (ts.peek() == Token(KW, "else")) {
    ts.read();
    ignoreWS();
    p->otherwise = read_statement();
    read_statement_end();
  }
  return p;
}

PrBody* Parser::read_block() {
  PrBody* p = new PrBody();
  ts.read(); // {
  while (ts.peek() != Token(PUNC,"}"))
    p->productions.push_back(read_production());
  ts.read(); // }
  return p;
}

void Parser::ignoreWS() {
  while (ts.peek() == Token(ENX,"\n"))
    ts.read();
}

PrFor* Parser::read_for() {
  PrFor* pfor = new PrFor();
  ts.read(); // for
  ignoreWS();
  ts.read(); //(
  
  ignoreWS();
  pfor->init = read_statement();
  ignoreWS();
  ts.read(); //;
  
  ignoreWS();
  pfor->condition = read_expression();
  ignoreWS();
  ts.read(); //;
  
  ignoreWS();
  if (ts.peek() != Token(PUNC,")")) {
    pfor->second = read_statement();
    ignoreWS();
    ts.read(); //)
  } else {
    pfor->second = new PrEmptyStatement(Token(ENX," "));
  }
  
  ignoreWS();
  pfor->first = read_statement();
  return pfor;
}

void Parser::read_statement_end() {
  bool read = false;
  if (ts.peek() == Token(ENX,";")) {
    read = true;
    ts.read();
  }
  if (ts.peek() == Token(ENX,"\n")) {
    read = true;
    ts.read();
  }
  // throw error if read is false
}
