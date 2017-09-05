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
    return new PrEmptyStatement();
  case KW:
    if (value == "var")
      return read_statement_var();
    if (value == "if")
      return read_statement_if();
    if (value == "for")
      return read_for();
    if (value == "while")
      return read_while();
    if (value == "with")
      return read_with();
    if (value == "switch")
      return read_switch();
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
  PrExpression* lhs = read_term();
  // check op of correct format:
  Token op = ts.read();
  PrExpression* rhs = 0;
  if (op.type != OPR) {
    rhs = read_expression();
  }
  PrAssignment* a = new PrAssignment(lhs,op,rhs);
  
  return a;
}

PrExpression* Parser::read_term() {
  PrExpression* to_return = 0;
  Token t(ts.peek());
  if (t == Token(OP,"-") || t == Token(OP,"!") || t == Token(KW,"not") || t.type == OPR)
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
  
  return read_accessors(to_return);
}

PrExpression* Parser::read_expression() {
  PrExpression* to_return = read_term();
  Token t(ts.peek());
  if (t.type == OP || t.type == OPR || t.is_op_keyword())
    to_return = read_arithmetic(to_return);
  return to_return;
} 

PrExpression* Parser::read_accessors(PrExpression* ds) {
  if (ts.peek() != Token(PUNC,"["))
    return ds;

  PrAccessorExpression* a = new PrAccessorExpression();
  a->ds = ds;
  ts.read(); // [
  ignoreWS();
  if (ts.peek().type == OPA || ts.peek() == Token(OP,"|"))
    a->acc = ts.read().value;
    
READ_INDEX:
  ignoreWS();
  a->indices.push_back(read_expression());
  ignoreWS();
  if (ts.peek() == Token(PUNC,",")) {
    ts.read();
    goto READ_INDEX;
  }
  
  ts.read(); // ]
  
  
  return read_accessors(a);
}

PrExprArithmetic* Parser::read_arithmetic(PrExpression* lhs) {
  //TODO assert ts.peek() is operator
  if (ts.peek() == Token(OP,"!") || ts.peek() == Token(KW,"not"))
    return nullptr; // TODO: better error handling
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
  ignoreWS();
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
  if (ts.peek() != Token(ENX,";"))
    pfor->condition = read_expression();
  else
    pfor->condition = nullptr;
  ignoreWS();
  ts.read(); //;
  
  ignoreWS();
  if (ts.peek() != Token(PUNC,")")) {
    pfor->second = read_statement();
    ignoreWS();
  } else {
    pfor->second = new PrEmptyStatement();
  }
  ts.read(); //)
  
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

PrWhile* Parser::read_while() {
  PrWhile* p = new PrWhile();
  ts.read(); // while
  ignoreWS();
  p->condition = read_expression();
  ignoreWS();
  p->event = read_statement();
  return p;
}

PrWith* Parser::read_with() {
  PrWith* p = new PrWith();
  ts.read(); // with
  ignoreWS();
  p->objid = read_expression();
  ignoreWS();
  p->event = read_statement();
  return p;
}

PrSwitch* Parser::read_switch() {
  PrSwitch* p = new PrSwitch();
  
  ts.read(); // switch
  ignoreWS();
  
  p->condition = read_expression();
  ignoreWS();
  
  ts.read(); // {
  ignoreWS();
  
  while (true) {
    if (ts.peek() == Token(PUNC,"}"))
        break;
        
    PrCase* c = new PrCase();
    Token t(ts.read()); // case
    ignoreWS();
    
    if (t==Token(KW,"case")) {
      c->value = read_expression();
      ignoreWS();
    } else
      c->value = nullptr;
    
    ts.read(); //:
    ignoreWS();
    
    while (ts.peek()!=Token(KW,"case") &&
           ts.peek()!=Token(KW,"default") &&
           ts.peek()!=Token(PUNC,"}")) {
      c->productions.push_back(read_production());
    }
    
    p->cases.push_back(c);
  }
  
  ts.read(); // }
  
  return p;
}
