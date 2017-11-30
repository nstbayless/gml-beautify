#include <vector>
#include <string>
#include <typeinfo>
#include <deque>

#include "tokenstream.h"
#include "beautify.h"
#include "lbstring.h"
#include "production.h"

#ifndef PARSER_H
#define PARSER_H

float get_op_priority(const Token&);

class Parser {
public:
  Parser(std::istream* is);
  Parser(std::string s);
  
  //! parses full stream
  PrBody* parse();
  
  //! parses a single line
  Production* read();
  
  //! ignore comments and whitespace
  bool ignore_decor = false;
private:
  Production* read_production();
  PrDecor* read_rawtoken();
  PrExpression* read_expression();
  PrExpression* read_term();
  PrExprParen* read_expression_parentheses();
  PrStatement* read_statement();
  PrAssignment* read_assignment();
  PrExpression* read_accessors(PrExpression* ds);
  PrExpression* read_possessive(PrExpression* owner);
  PrExprArithmetic* read_arithmetic(PrExpression* lhs);
  PrExpressionFn* read_expression_function();
  PrStatementFn* read_statement_function();
  PrStatementVar* read_statement_var();
  PrStatementIf* read_statement_if();
  PrBody* read_block(bool braces = true);
  PrFor* read_for();
  PrWith* read_with();
  PrWhile* read_while();
  PrRepeat* read_repeat();
  PrDo* read_do();
  PrSwitch* read_switch();
  void assert_peek(Token t, std::string message) const;
  
  //! read comments and whitespaces as infixes for p
  void ignoreWS(Production* p, bool as_postfix = false);
  
  //! take any postfixes of src and apply them as infixes (postfixes, if as_postfix) to dst
  void siphonWS(Production* src, Production* dst, bool as_postfix = false, bool condense = false);
  
  //! removes final newline from end of postfixes
  void removeExtraNewline(Production* p);
  
  //! read semicolon and/or line ending
  void read_statement_end();
  
  LLKTokenStream ts;
};

#endif /*PARSER_H*/
