#include "parser.h"

using namespace std;

Parser::Parser(istream* is): ts(is, 4) { }

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
  // read comments before final semicolon
  while (ts.peek().type == COMMENT) {
    p->infixes.push_back(new PrInfixWS(ts.read()));
  }
  read_statement_end();
  ignoreWS(p, true);
  removeExtraNewline(p);
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
  case OPR:
    return read_assignment();
  }
}

PrAssignment* Parser::read_assignment() {
  if (ts.peek().type == OPR) {
    Token op = ts.read();
    PrExpression* lhs = read_term();
    return new PrAssignment(lhs,op,nullptr);
  } else {
    PrExpression* lhs = read_term();
    // check op of correct format:
    Token op = ts.read();
    PrAssignment* p = new PrAssignment(lhs,op,nullptr);
    ignoreWS(p);
    PrExpression* rhs = 0;
    if (op.type != OPR) {
      rhs = read_expression();
      siphonWS(rhs,p,true);
    }
    p->rhs = rhs;
    return p;
  }
}

PrExpression* Parser::read_term() {
  PrExpression* to_return = 0;
  Token t(ts.peek());
  if (t == Token(OP,"-")   || t == Token(OP,"!") ||
      t == Token(KW,"not") || t == Token(OP,"~") || t.type == OPR) {
    PrExprArithmetic* p = new PrExprArithmetic(nullptr, ts.read(),read_expression());
    siphonWS(p->rhs,p,true);
  } else {
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
    
    // read postfixes
    ignoreWS(to_return,true);
    
    return read_possessive(read_accessors(to_return));
  }
}

PrExpression* Parser::read_possessive(PrExpression* owner) {
  if (ts.peek() == Token(PUNC, ".")) {
    PrExprArithmetic* p = new PrExprArithmetic(owner,ts.read(),read_term());
    siphonWS(p->rhs,p,true);
    return p;
  } else
    return owner;
}

PrExpression* Parser::read_accessors(PrExpression* ds) {
  if (ts.peek() != Token(PUNC,"["))
    return ds;

  PrAccessorExpression* a = new PrAccessorExpression();
  siphonWS(ds, a, false, true);
  a->ds = ds;
  ts.read(); // [
  ignoreWS(ds);
  if (ts.peek().type == OPA || ts.peek() == Token(OP,"|"))
    a->acc = ts.read().value;
    
READ_INDEX:
  ignoreWS(ds);
  a->indices.push_back(read_expression());
  ignoreWS(ds);
  if (ts.peek() == Token(PUNC,",")) {
    ts.read();
    goto READ_INDEX;
  }
  
  ts.read(); // ]
  
  return read_accessors(a);
}

PrExpression* Parser::read_expression() {
  PrExpression* to_return = read_term();
  ignoreWS(to_return, true);
  Token t(ts.peek());
  if (t.type == OP || t.type == OPR || t.is_op_keyword())
    to_return = read_arithmetic(to_return);
  return to_return;
} 

PrExprArithmetic* Parser::read_arithmetic(PrExpression* lhs) {
  //TODO assert ts.peek() is operator
  if (ts.peek() == Token(OP,"!") || ts.peek() == Token(KW,"not"))
    return nullptr; // TODO: better error handling
  if (ts.peek().type == OPR) {
    new PrExprArithmetic(lhs, ts.read(), nullptr);
  }
  Token op = ts.read();
  PrExprArithmetic* p = new PrExprArithmetic(lhs, op, nullptr);
  ignoreWS(p);
  p->rhs = read_expression();
  siphonWS(p->rhs,p,true);
  return p;
}

PrExpressionFn* Parser::read_expression_function() {
  PrExpressionFn* pfn = new PrExpressionFn(ts.read());
  
  ignoreWS(pfn);
  // (
  ts.read();  

  while (true) {
    ignoreWS(pfn);
    Token next(ts.peek());
    if (next == Token(PUNC,")"))
      break;
    pfn->args.push_back(read_expression());
    ignoreWS(pfn);
    next = ts.peek();
    if (next == Token(PUNC,",")) {
      ts.read();
      continue;
    }
    else break;
  }
  
  ts.read(); // )
  
  ignoreWS(pfn, true);
  
  return pfn;
}

PrStatementFn* Parser::read_statement_function() {
  PrStatementFn* fn = new PrStatementFn();
  fn->fn = read_expression_function();
  siphonWS(fn->fn,fn,true);
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
    ignoreWS(p);
    PrVarDeclaration* d = new PrVarDeclaration(ts.read());
    ignoreWS(p);
    if (ts.peek() == Token(OP,"=")) {
      ts.read();
      ignoreWS(p);
      d->definition = read_expression();
    }
    p->declarations.push_back(d);
    ignoreWS(p);
    if (ts.peek() == Token(PUNC,",")) {
      ts.read();
      continue;
    }
    break;
  }
  
  return p;
}

PrStatementIf* Parser::read_statement_if() {
  PrStatementIf* p = new PrStatementIf();
  ts.read(); // read IF
  ignoreWS(p);
  p->condition = read_expression();
  siphonWS(p->condition,p,false,true);
  
  p->result = read_statement();
  siphonWS(p->result,p,true);
  if (ts.peek() == Token(KW, "else")) {
    // what were previously postfixes now count as infixes, since we're extending
    p->postfix_n = 0;
    ts.read();
    ignoreWS(p);
    p->otherwise = read_statement();
    read_statement_end();
    ignoreWS(p, true);
    removeExtraNewline(p);
  }
  return p;
}

PrBody* Parser::read_block() {
  PrBody* p = new PrBody();
  ts.read(); // {
  ignoreWS(p);
  while (ts.peek() != Token(PUNC,"}"))
    p->productions.push_back(read_production());
  ts.read(); // }
  return p;
}

void Parser::ignoreWS(Production* p, bool as_postfix) {  
  if (ts.peek() == Token(ENX,"\n") || ts.peek().type == COMMENT) {
    PrInfixWS* infix = new PrInfixWS(ts.read());
    ignoreWS(infix);
    p->infixes.push_back(infix);
  } else {
    p->infixes.push_back(nullptr);  
  }
  p->postfix_n += as_postfix;
}

//! takes all postfixes from src and inserts them into dst
void Parser::siphonWS(Production* src, Production* dst, bool as_postfix, bool condense) {
  int N = src->postfix_n;
  PrInfixWS* infixes[max(N,1)];
  
  // remove infixes from src
  while (src->postfix_n > 0) {
    infixes[--src->postfix_n] = src->infixes.back();
    src->infixes.pop_back();
  }
  
  // condense all siphoned infixes into a single infix (if condense is true)
  if (condense) {
    // find first not-null postfix
    int first_non_null = 0;
    for (int i=0;i<N;i++) {
      if (infixes[i] == nullptr)
        first_non_null++;
      else
        break;
    }
    
    // attach other postfixes as postfixes to the first non-null postfix:
    for (int i=first_non_null+1;i<N;i++) {
      infixes[first_non_null]->infixes.push_back(infixes[i]);
      infixes[first_non_null]->postfix_n++;
    }
    
    // edge cases:
    if (N==0) {
      infixes[0] = nullptr;
    } else if (first_non_null < N) {
      infixes[0] = infixes[first_non_null];
    }
    N = 1;
  }
  
  // append siphoned infixes to dst
  for (int i=0;i<N;i++) {
    dst->infixes.push_back(infixes[i]);
    dst->postfix_n += as_postfix;
  }
}

void Parser::removeExtraNewline(Production* p) {
  //TODO: rewrite this to use iterators
  auto& infixes = p->infixes;
  auto& postfix_n = p->postfix_n;
  
  p->flattenPostfixes();
  
  for (int i=0;i<postfix_n;i++) {
    int iter = infixes.size() - i - 1;
    if (infixes[iter]) {
      if (infixes[iter]->val.value == "\n") {
        delete(infixes[iter]);
        infixes[iter] = nullptr;
        return;
      } else {
        return;
      }
    }
  }
}

PrFor* Parser::read_for() {
  PrFor* pfor = new PrFor();
  ts.read(); // for
  ignoreWS(pfor);
  ts.read(); //(
  
  ignoreWS(pfor);
  pfor->init = read_statement();
  ignoreWS(pfor);
  ts.read(); //;
  
  ignoreWS(pfor);
  if (ts.peek() != Token(ENX,";"))
    pfor->condition = read_expression();
  else
    pfor->condition = nullptr;
  ignoreWS(pfor);
  ts.read(); //;
  
  ignoreWS(pfor);
  if (ts.peek() != Token(PUNC,")")) {
    pfor->second = read_statement();
    ignoreWS(pfor);
  } else {
    pfor->second = new PrEmptyStatement();
  }
  ts.read(); //)
  
  ignoreWS(pfor, true);
  pfor->first = read_statement();
  return pfor;
}

void Parser::read_statement_end() {
  bool read = false;
  if (ts.peek() == Token(ENX,";")) {
    read = true;
    ts.read();
  }
}

PrWhile* Parser::read_while() {
  PrWhile* p = new PrWhile();
  ts.read(); // while
  ignoreWS(p);
  p->condition = read_expression();
  siphonWS(p->condition, p, false, true);
  p->event = read_statement();
  return p;
}

PrWith* Parser::read_with() {
  PrWith* p = new PrWith();
  ts.read(); // with
  ignoreWS(p);
  p->objid = read_expression();
  siphonWS(p->objid, p, false, true);
  p->event = read_statement();
  return p;
}

PrSwitch* Parser::read_switch() {
  PrSwitch* p = new PrSwitch();
  
  ts.read(); // switch
  ignoreWS(p);
  
  p->condition = read_expression();
  ignoreWS(p);
  
  ts.read(); // {
  ignoreWS(p);
  
  while (true) {
    if (ts.peek() == Token(PUNC,"}"))
        break;
        
    PrCase* c = new PrCase();
    Token t(ts.read()); // case
    ignoreWS(p);
    
    if (t==Token(KW,"case")) {
      c->value = read_expression();
      ignoreWS(p);
    } else {
      ts.read(); // default
      c->value = nullptr;
    }
    
    ts.read(); //:
    ignoreWS(p);
    
    while (ts.peek()!=Token(KW,"case") &&
           ts.peek()!=Token(KW,"default") &&
           ts.peek()!=Token(PUNC,"}")) {
      c->productions.push_back(read_production());
      ignoreWS(p);
    }
    p->cases.push_back(c);
  }
  
  ts.read(); // }
  
  return p;
}
