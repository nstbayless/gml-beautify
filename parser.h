#include "tokenstream.h"
#include <vector>
#include <string>

enum PrType {
}

struct Production {
  virtual string to_string();
  virtual PrType get_type();
}

struct PrDecor: Production {
  virtual string to_string();
  virtual PrType get_type();
  
  Token rawToken;
}

struct PrExpression: Production {
}

// parentheses are important to remember for beautifier
struct PrExprParen: Production {
  virtual string to_string();
  virtual PrType get_type();
  
  PrExpression* content;
}

struct PrExpressionFn: PrExpression{
  virtual string to_string();
  virtual PrType get_type();
  
  Token fn;
  vector<PrExpression*> args;
}

struct PrExprArithmetic: PrExpression {
  virtual string to_string();
  virtual PrType get_type();
  
  PrExpression* lhs;
  PrExpression *rhs;
  Token op;
}

struct PrStatement: Production {
}

struct PrAssignable: Production {
  virtual string to_string();
  virtual PrType get_type();
  
  Token identifier;
}

struct PrAssignment: PrStatement {
  virtual string to_string();
  virtual PrType get_type();
  
  PrAssignable* lhs;
  PrExpression* rhs;
  Token op;
}

struct PrStatementFn: PrStatement {
  virtual string to_string();
  virtual PrType get_type();
  
  PrExpressionFn* fn;
}

struct prBody: Production {
  virtual string to_string();
  virtual PrType get_type();
  
  vector<Production*> productions;
}

class Parser {
public:
  Parser(istream* is);
  Production* read();
private:
  Production* read_production();
  PrDecor* read_rawtoken();
  PrExpression* read_expression();
  PrExprParen* read_expression_parentheses();
  PrStatement* read_statement();
  PrAssignment* read_assignment();
  PrAssignable* read_assignable();
  PrExprArithmetic* read_arithmetic(PrExpression* lhs);
  PrExpressionFn* read_expression_function();
  PrStatementFn* read_statement_function();
  
  LLKTokenStream* ts;
}
