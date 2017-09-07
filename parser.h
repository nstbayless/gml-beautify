#include <vector>
#include <string>
#include <typeinfo>

#include "tokenstream.h"

#ifndef PARSER_H
#define PARSER_H

struct BeautifulConfig {
  //! put open brace on same line
  bool egyptian = false;
  
  //! put semicolons at the end of each line
  bool semicolons = true;
  
  //! indent using spaces
  bool indent_spaces = true;
  
  //! number of spaces to indent with
  int indent_spaces_per_tab = 4;
  
  //! put spacing around ++/-- operator
  bool opr_space = false;
  
  //! put spacing after ! and ~ operators
  bool not_space = false;
  
  //! put spacing after accessor symbol, e.g. array[@ index]
  bool accessor_space = true;
  
// these options can modify the tokens:

  //! parens for if statement condition (-1: leave untouched)
  int if_parens = -1;
  
  //! parens for while loop condition
  int while_parens = 1;

  //! parens for object id in with statement
  int with_parens = 0;
};

struct BeautifulContext {
  //! number of indents
  int depth = 0;
  
  //! do not indent
  bool condense = false;
  
  //! is inline (do not indent and do not append semicolon)
  bool is_inline = false;
  
  //! block is attached to if/while/with/etc.
  bool attached = false;
  
  BeautifulContext increment_depth() const;
  BeautifulContext decrement_depth() const;
  BeautifulContext as_condensed() const;
  BeautifulContext as_inline() const;
  BeautifulContext not_inline() const;
  
  // floating block control:
  BeautifulContext attach() const;
  BeautifulContext detach() const;
};

const BeautifulContext DEFAULT_CONTEXT;

struct PrInfixWS;

enum InfixStyle {
  //! single line; convert comments to /* comment */
  SL_CONVERT_SML,
  
  //! single line; convert comments to /* comment */, leave /* */ untouched even if multi-line
  SL_CONVERT_ML,
  
  //! one or two line; convert comments to /* comment */ except last comment; add newline if // comment
  TWO_LINES,
  
  //! leave as-is but if newline, adjust indenting to match line's indent
  ML_INDENT,
  
  //! leave as-is
  AS_IS
}

struct Production {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&,
    BeautifulContext bc = DEFAULT_CONTEXT) const;
    
protected:
  retrieveWS()
};

struct PrDecor: Production {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext bc) const;
  
  PrDecor(Token rawToken);
  
  Token rawToken;
};

struct PrExpression: Production {
};

// parentheses are important to remember for beautifier
struct PrExprParen: PrExpression {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext bc) const;
  
  PrExpression* content;
};

struct PrExpressionFn: PrExpression{
  PrExpressionFn(Token identifier);
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  Token identifier;
  std::vector<PrExpression*> args;
};

struct PrExprArithmetic: PrExpression {
  PrExprArithmetic(PrExpression* lhs, Token op, PrExpression* rhs);
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  PrExpression* lhs;
  PrExpression *rhs;
  Token op;
};

struct PrStatement: Production {
};

struct PrEmptyStatement: PrStatement {
  PrEmptyStatement();
  PrEmptyStatement(Token enx);
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  bool hastoken;
  Token enx;
};

struct PrFinal: PrExpression {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  PrFinal(Token t);
  
  Token final;
};

struct PrIdentifier: PrExpression {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  PrIdentifier(Token t);
  
  Token identifier;
};

struct PrAssignment: PrStatement {
  PrAssignment(PrExpression* lhs, Token op, PrExpression* rhs);
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  PrExpression* lhs;
  PrExpression* rhs;
  Token op;
};

struct PrStatementFn: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  PrExpressionFn* fn;
};

struct PrVarDeclaration: Production {
  PrVarDeclaration(Token ident,  PrExpression* def = nullptr);
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  Token identifier;
  PrExpression* definition;
};

struct PrStatementVar: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  std::vector<PrVarDeclaration*> declarations;
};

struct PrBody: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  std::vector<Production*> productions;
};

struct PrStatementIf: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  PrExpression* condition;
  PrStatement* result;
  PrStatement* otherwise=nullptr;
};

struct PrFor: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  PrStatement* init;
  PrExpression* condition;
  PrStatement* second;
  PrStatement* first;
};

struct PrWhile: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  PrExpression* condition;
  PrStatement* event;
};

struct PrWith: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  PrExpression* objid;
  PrStatement* event;
};

struct PrAccessorExpression: PrExpression {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  std::string acc = "";
  PrExpression* ds;
  std::vector<PrExpression*> indices;
};


struct PrCase: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  PrExpression* value;
  std::vector<Production*> productions;
};

struct PrSwitch: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  PrExpression* condition;
  std::vector<PrCase*> cases;
};

struct PrControl: PrStatement {
  PrControl(Token,PrExpression* val = nullptr);
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  Token kw;
  PrExpression* val;
};

struct PrInfixWS: Production {
  PrInfixWS(Token);
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig& ,BeautifulContext) const;
  
  Token val;
}

class Parser {
public:
  Parser(std::istream* is);
  Production* read();
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
  PrBody* read_block();
  PrFor* read_for();
  PrWith* read_with();
  PrWhile* read_while();
  PrSwitch* read_switch();
  
  void ignoreWS(Production* p);
  void read_statement_end();
  
  LLKTokenStream ts;
};

#endif /*PARSER_H*/
