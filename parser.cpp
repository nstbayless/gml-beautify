#include "parser.h"

Parser::Parser(istream* is): ts(is, 4) {

}

Production* Parser::read() {
  if (ts->peek().type == END || ts->peek().type == ERR)
    return nullptr;
  return read_production();
}

Production* Parser::read_production() {
  TokenType t = ts->peek().type;
  if (t == COMMENT || t == WS)
    return read_rawtoken();
  return read_statement();
}

PrDecor* Parser::read_rawtoken() {
  PrDecor* p = new PrDecor();
  p->rawToken = ts->read();
}

PrStatement* Parser::read_statement() {  
  string val(ts->peek().value)
  switch (ts->peek().type) {
  case KW:
    //TODO:
    return nullptr;
  case PUNC:
    if (value == "(") {
      return read_assignment();
    }
    if (value == "{") {
      return read_body();
    }
    return nullptr;
  case ID:
    {
      Token t = ts->peek(1);
      if (t == Token(PUNC,"("))
        return read_statement_function();
      return read_assignment();
    }
  }
}

PrAssignment* Parser::read_assignment() {
  PrAssignable* lhs = read_assignable();
  Token op = ts->read();
  PrExpression* rhs = 0;
  if (op.type != OPR) {
    rhs = read_expression();
  }
  PrAssignment* a = new PrAssignment();
  a->lhs = lhs;
  a->op = op;
  a->rhs = rhs;
  
  // read ENX:
  ts -> read();
  
  return a;
}

PrAssignable* Parser::read_assignable() {
  Token t(ts->peek());
  PrAssignable* a;
  if (t == Token(PUNC,"(")) {
    // TODO
    return 0;
  } else {
    t = ts->read();
    a = new PrAssignable();
    a->identifier = t;
  }
  // optional: . or []
  
  return a;
}

PrExpression* Parser::read_expression() {
  PrExpression* to_return = 0;

  Token t(ts->peek());
  if (t == Token(PUNC,"("))
    to_return = read_expression_parentheses();
  else {
    t = ts->peek(1)
    if (t == Token(PUNC,"("))
      to_return read_expression_function();
  }
  t = ts->peek();
  if (t.type == OP)
    return read_arithmetic(to_return)
  else
    return to_return;
} 

PrExprArithmetic* Parser::read_arithmetic(PrExpression* lhs) {
  PrExpressionArithemtic* a = new PrExpressionArithmetic();
  a->lhs = lhs;
  a->op = ts->read(); //TODO: assert is OP
  a->rhs = read_expression();
  //TODO: rebalance order-of-operations
  return a;
}

PrExpressionFn* Parser::read_expression_function() {
  PrExpressionFn* pfn = new PrExpressionFn();

  pfn->fn = ts->read();
  
  // read paren:
  ts->read();  
  
  while (true) {
    Token next(ts->peek());
    if (next == Token(PUNC,")"))
      break;
    pfn->args.push_back(read_expression());
    next = ts->read();
    if (next == Token(PUNC,","))
      continue;
    // assert is end-parenthesis
  }
  
  return pfn;
}

PrStatementFn* Parser::read_statement_function() {
  PrStatementFunction* fn = new PrStatementFunction();
  fn->fn = read_expression_function();
  return fn;
}

PrExprParen* Praser::read_expression_parentheses() {
  PrExprParen* p = new PrExprParen();
  ts->read();
  p->content = read_expression();
  ts->read();
  return p;
}
