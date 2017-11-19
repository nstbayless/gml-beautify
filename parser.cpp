#include "parser.h"
#include "error.h"
#include "util.h"

using namespace std;

Parser::Parser(istream* is): ts(is, 2) { }
Parser::Parser(std::string s): ts(s, 2) { }

void Parser::assert_peek(Token t, std::string message) const {
  if (ts.peek().type != t.type) {
    throw ParseError(replace_all(message, "%unexpected", "unexpected " + std::string(TOKEN_NAME_PLAIN[ts.peek().type])), ts.location());
  }
  if (ts.peek().value != t.value) {
    throw ParseError(replace_all(message, "%unexpected", "unexpected token \"" + ts.peek().value + "\""), ts.location());
  }
}

Production* Parser::read() {
  if (ts.peek().type == END || ts.peek().type == ERR)
    return nullptr;
  return read_production();
}

PrBody* Parser::parse() {
  return read_block(false);
}

Production* Parser::read_production() {
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
  case WS:
  case COMMENT:
  case ENX:
    return new PrEmptyStatement();
  case KW:
    if (value == "var" || value == "globvar")
      return read_statement_var();
    else if (value == "if")
      return read_statement_if();
    else if (value == "for")
      return read_for();
    else if (value == "while")
      return read_while();
    else if (value == "do")
      return read_do();
    else if (value == "with")
      return read_with();
    else if (value == "switch")
      return read_switch();
    else if (value == "return") {
      auto* p = new PrControl(ts.read());
      ignoreWS(p);
      p->val = read_expression();
      siphonWS(p->val, p, true);
      return p;
    } else if (value == "exit" || value == "continue" || value == "break") {
      auto* p = new PrControl(ts.read());
      ignoreWS(p, true);
      return p;
    }
    else {
      throw ParseError("keyword " + value + " cannot start a statement.", ts.location());
    }
  case PUNC:
    if (value == "(") {
      return read_assignment();
    }
    if (value == "{") {
      return read_block();
    }
    throw ParseError("unexpected punctuation \"" + value + "\" where a statement was expected.", ts.location());
  case ID:
    {
      Token t = ts.peek(1);
      if (t == Token(PUNC,"("))
        return read_statement_function();
      return read_assignment();
    }
  case OPR:
    return read_assignment();
  default:
    throw ParseError("unexpected token \"" + value + "\" when a statement was expected instead.", ts.location());
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
    if (ts.peek().type != OPR && ts.peek().type != OP) {
      throw ParseError("unexpected token \"" + ts.peek().value
          + "\" where an assignment operator was expected.", ts.location());
    }
    // read operator
    Token op = ts.read();
    PrAssignment* p = new PrAssignment(lhs,op,nullptr);
    ignoreWS(p);
    PrExpression* rhs = nullptr;
    if (op.type != OPR) {
      rhs = read_expression();
      p->postfix_n = 0;
      siphonWS(rhs,p,true);
    }
    p->rhs = rhs;
    return p;
  }
}

PrExpression* Parser::read_term() {
  PrExpression* to_return = 0;
  Token t(ts.peek());
  if (t == Token(OP,"-")   || t == Token(OP,"!") || t == Token(OP,"+")
      || t == Token(KW,"not") || t == Token(OP,"~") || t.type == OPR) {
    // unary operator term
    PrExprArithmetic* p = new PrExprArithmetic(nullptr, ts.read(),read_expression());
    siphonWS(p->rhs,p,true);
    return p;
  } else {
    if (t.type == NUM || t.type == STR) {
      // literal
      to_return = new PrFinal(ts.read());
    }
    if (t == Token(PUNC,"(")) {
      // parentheses expression
      to_return = read_expression_parentheses();
    }
    else if (t.type == ID) {
      // function or identifier
      t = ts.peek(1);
      if (t == Token(PUNC,"("))
        to_return = read_expression_function();
      else
        to_return = new PrIdentifier(ts.read());
    }
    
    // read postfixes
    ignoreWS(to_return,true);
    
    // optional right-hand modifiers (array, .)
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
  // TODO: assert valid accessor symbol
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
  
  assert_peek(Token(PUNC,"]"), "%unexpected while parsing accessor; either \",\" or \"]\" expected");
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
  if (ts.peek() == Token(OP,"!") || ts.peek() == Token(KW,"not") || ts.peek() == Token(OP,"~"))
    throw ParseError("unexpected unary operator after expression", ts.location());
  if (ts.peek().type == OPR) {
    return new PrExprArithmetic(lhs, ts.read(), nullptr);
  }
  Token op = ts.read();
  PrExprArithmetic* p = new PrExprArithmetic(lhs, op, nullptr);
  siphonWS(lhs,p,false,true);
  ignoreWS(p);
  p->rhs = read_expression();
  siphonWS(p->rhs,p,true);
  return p;
}

PrExpressionFn* Parser::read_expression_function() {
  PrExpressionFn* pfn = new PrExpressionFn(ts.read());
  
  ignoreWS(pfn);
  
  // (
  assert_peek(Token(PUNC,"("),"%unexpected while expecting open-parenthesis \"(\" for function");
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
  
  // )
  assert_peek(Token(PUNC,")"),"%unexpected while parsing function; expected \",\" or \")\"");
  ts.read();
  
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
  
  assert_peek(Token(PUNC,"("),"%unexpected while expecting open parenthesis \"(\"");
  ts.read(); //(
  
  p->content = read_expression();
  
  assert_peek(Token(PUNC,")"),"%unexpected while expecting matching close parenthesis \"(\"");
  ts.read(); //)
  return p;
}

PrStatementVar* Parser::read_statement_var() {
  PrStatementVar* p = new PrStatementVar();
  if (ts.peek() != Token(KW,"globvar"))
    assert_peek(Token(KW,"var"),"%unexpected while expecting var declaration");
  p->type = ts.read().value; // read "var"
  while (true) {
    ignoreWS(p);
    if (ts.peek().type != ID)
      throw ParseError("Unexpected token \"" + ts.peek().value + "\" while reading var declaration; expected variable name.", ts.location());
    PrVarDeclaration* d = new PrVarDeclaration(ts.read());
    ignoreWS(d);
    if (ts.peek() == Token(OP,"=")) {
      ts.read();
      ignoreWS(d);
      d->definition = read_expression();
    }
    ignoreWS(p);
    p->declarations.push_back(d);
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
  assert_peek(Token(KW,"if"), "%unexpected; expected if statement");
  ts.read(); // read IF
  ignoreWS(p);
  p->condition = read_expression();
  siphonWS(p->condition,p,false,true);
  
  p->result = read_statement();
  read_statement_end();
  ignoreWS(p->result,true);
  siphonWS(p->result,p,true,true);
  
  if (ts.peek() == Token(KW, "else")) {
    // what were previously postfixes now count as infixes, since we're extending
    p->postfix_n = 0;
    ts.read();
    ignoreWS(p);
    p->otherwise = read_statement();
    siphonWS(p->otherwise, p, true);
    read_statement_end();
    ignoreWS(p, true);
  }
  return p;
}

PrBody* Parser::read_block(bool braces) {
  PrBody* p = new PrBody();
  p->is_root = !braces;
  
  if (braces) {
    assert_peek(Token(PUNC,"{"), "expected open brace, \"{\"");
    ts.read(); // {
  }
  
  // read productions inside of braces
  while (ts.peek() != Token(PUNC,"}") && !ts.eof())
    p->productions.push_back(read_production());
  
  if (braces) {
    assert_peek(Token(PUNC,"}"), "expected matching end brace, \"}\"");
    ts.read(); // }
  }
  ignoreWS(p, true);
  
  return p;
}

void Parser::ignoreWS(Production* p, bool as_postfix) {
  if (ts.peek() == Token(ENX,"\n") || ts.peek().type == COMMENT) {
    PrInfixWS* infix = new PrInfixWS(ts.read());
    ignoreWS(infix);
    if (!ignore_decor)
      p->infixes.push_back(infix);
    else {
      delete(infix);
      p->infixes.push_back(nullptr);
    }
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
  assert_peek(Token(KW,"for"), "%unexpected; expected \"for\" statement.");
  ts.read(); // for
  ignoreWS(pfor);
  assert_peek(Token(PUNC,"("), "%unexpected where open parenthesis \"(\" for for statement arguments expected.");
  ts.read(); //(
  
  ignoreWS(pfor);
  pfor->init = read_statement();
  read_statement_end();
  ignoreWS(pfor);
  
  if (ts.peek() != Token(ENX,";"))
    pfor->condition = read_expression();
  else
    pfor->condition = nullptr;
  read_statement_end();
  
  ignoreWS(pfor);
  if (ts.peek() != Token(PUNC,")")) {
    pfor->second = read_statement();
    read_statement_end();
  } else {
    pfor->second = new PrEmptyStatement();
  }
  ignoreWS(pfor);
  
  assert_peek(Token(PUNC,")"), "%unexpected where end parenthesis \")\" for for statement arguments expected.");
  ts.read(); //)
  
  ignoreWS(pfor);
  pfor->first = read_statement();
  siphonWS(pfor->first, pfor, true);
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
  assert_peek(Token(KW,"while"), "%unexpected; expected \"while\" statement.");
  ts.read(); // while
  ignoreWS(p);
  p->condition = read_expression();
  siphonWS(p->condition, p, false, true);
  p->event = read_statement();
  siphonWS(p->event, p, true);
  return p;
}

PrDo* Parser::read_do() {
  PrDo* p = new PrDo();
  assert_peek(Token(KW,"do"), "%unexpected; expected \"do\" statement.");
  ts.read(); // do
  ignoreWS(p);
  p->event = read_statement();
  siphonWS(p->event, p, false, true);
  assert_peek(Token(KW,"until"), "%unexpected where \"until\" needed following \"do\"");
  ts.read(); // until
  ignoreWS(p);
  p->condition = read_expression();
  siphonWS(p->condition, p, true);
  return p;
}


PrWith* Parser::read_with() {
  PrWith* p = new PrWith();
  assert_peek(Token(KW,"with"), "%unexpected; expected \"with\" statement.");
  ts.read(); // with
  ignoreWS(p);
  p->objid = read_expression();
  siphonWS(p->objid, p, false, true);
  p->event = read_statement();
  siphonWS(p->event, p, true);
  return p;
}

PrSwitch* Parser::read_switch() {
  PrSwitch* p = new PrSwitch();
  
  assert_peek(Token(KW,"switch"), "%unexpected; expected \"switch\" statement.");
  ts.read(); // switch
  ignoreWS(p);
  
  p->condition = read_expression();
  siphonWS(p->condition, p, false, true);
  
  assert_peek(Token(PUNC,"{"), "%unexpected where open brace \"{\" required for switch statement.");
  ts.read(); // {
  ignoreWS(p);
  
  while (true) {
    if (ts.peek() == Token(PUNC,"}"))
        break;
        
    PrCase* c = new PrCase();
    if (ts.peek() != Token(KW,"default"))
      assert_peek(Token(KW,"case"), "%unexpected; switch statement requires cases.");
    Token t(ts.read()); // case
    ignoreWS(c);
    
    if (t==Token(KW,"case")) {
      c->value = read_expression();
      ignoreWS(c);
    } else {
      c->value = nullptr;
    }
    
    assert_peek(Token(PUNC,":"), "%unexpected where colon \":\" required for case.");
    ts.read(); //:
    ignoreWS(c);
    
    while (ts.peek()!=Token(KW,"case") &&
           ts.peek()!=Token(KW,"default") &&
           ts.peek()!=Token(PUNC,"}")) {
      c->productions.push_back(read_production());
    }
    if (!c->productions.empty())
      siphonWS(c->productions.back(), c, false, true);
    p->cases.push_back(c);
  }
  
  assert_peek(Token(PUNC,"}"), "%unexpected; expected matching close brace \"}\" for switch statement.");
  ts.read(); // }
  ignoreWS(p, true);
  
  return p;
}