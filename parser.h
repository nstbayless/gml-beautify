#include "tokenstream.h"
#include <vector>
#include <string>

#ifndef PARSER_H
#define PARSER_H

enum PrType {
  PROD
};

struct Production {
  virtual std::string to_string();
  virtual PrType get_type();
};

struct PrDecor: Production {
  virtual std::string to_string();
  virtual PrType get_type();
  
  PrDecor(Token rawToken);
  
  Token rawToken;
};

struct PrExpression: Production {
};

// parentheses are important to remember for beautifier
struct PrExprParen: PrExpression {
  virtual std::string to_string();
  virtual PrType get_type();
  
  PrExpression* content;
};

struct PrExpressionFn: PrExpression{
  PrExpressionFn(Token identifier);
  virtual std::string to_string();
  virtual PrType get_type();
  
  Token identifier;
  std::vector<PrExpression*> args;
};

struct PrExprArithmetic: PrExpression {
  PrExprArithmetic(PrExpression* lhs, Token op, PrExpression* rhs);
  virtual std::string to_string();
  virtual PrType get_type();
  
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
  virtual PrType get_type();
  
  bool hastoken;
  Token enx;
};

struct PrFinal: PrExpression {
  virtual std::string to_string();
  virtual PrType get_type();
  
  PrFinal(Token t);
  Token final;
};

struct PrIdentifier: PrExpression {
  virtual std::string to_string();
  virtual PrType get_type();
  PrIdentifier(Token t);
  
  Token identifier;
};

struct PrAssignment: PrStatement {
  PrAssignment(PrExpression* lhs, Token op, PrExpression* rhs);
  virtual std::string to_string();
  virtual PrType get_type();
  
  PrExpression* lhs;
  PrExpression* rhs;
  Token op;
};

struct PrStatementFn: PrStatement {
  virtual std::string to_string();
  virtual PrType get_type();
  
  PrExpressionFn* fn;
};

struct PrVarDeclaration: Production {
  PrVarDeclaration(Token ident,  PrExpression* def = nullptr);
  virtual std::string to_string();
  virtual PrType get_type();
  
  Token identifier;
  PrExpression* definition;
};

struct PrStatementVar: PrStatement {
  virtual std::string to_string();
  virtual PrType get_type();
  
  std::vector<PrVarDeclaration*> declarations;
};

struct PrStatementIf: PrStatement {
  virtual std::string to_string();
  virtual PrType get_type();
  
  PrExpression* condition;
  PrStatement* result;
  PrStatement* otherwise=nullptr;
};

struct PrBody: PrStatement {
  virtual std::string to_string();
  virtual PrType get_type();
  
  std::vector<Production*> productions;
};

struct PrControl: PrStatement {
  PrControl(Token,PrExpression* val = nullptr);
  virtual std::string to_string();
  virtual PrType get_type();
  
  Token kw;
  PrExpression* val;
};

struct PrFor: PrStatement {
  virtual std::string to_string();
  virtual PrType get_type();
  
  PrStatement* init;
  PrExpression* condition;
  PrStatement* second;
  PrStatement* first;
};

struct PrWhile: PrStatement {
  virtual std::string to_string();
  virtual PrType get_type();
  
  PrExpression* condition;
  PrStatement* event;
};

struct PrWith: PrStatement {
  virtual std::string to_string();
  virtual PrType get_type();
  
  PrExpression* objid;
  PrStatement* event;
};

struct PrAccessorExpression: PrExpression {
  virtual std::string to_string();
  virtual PrType get_type();
  
  std::string acc = "";
  PrExpression* ds;
  std::vector<PrExpression*> indices;
};


struct PrCase: PrStatement {
  virtual std::string to_string();
  virtual PrType get_type();
  
  PrExpression* value;
  std::vector<Production*> productions;
};

struct PrSwitch: PrStatement {
  virtual std::string to_string();
  virtual PrType get_type();
  
  PrExpression* condition;
  std::vector<PrCase*> cases;
};

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
  
  void ignoreWS();
  void read_statement_end();
  
  LLKTokenStream ts;
};

#endif /*PARSER_H*/
