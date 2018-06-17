#include <vector>
#include <string>
#include <typeinfo>
#include <deque>

#include "tokenstream.h"
#include "beautify.h"
#include "lbstring.h"

#ifndef PRODUCTION_H
#define PRODUCTION_H

struct CompilerContext;

struct Production {
  friend class Parser;
  virtual ~Production();
  
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const = 0;
  virtual LBString beautiful(const BeautifulConfig&,
    BeautifulContext bc = DEFAULT_CONTEXT);
  
  LBString renderWS(const BeautifulConfig&,
    BeautifulContext bc);
  
  LBString renderPostfixesTrimmed(const BeautifulConfig&,
    BeautifulContext bc);
  
  
  void flattenPostfixes();
  
  std::deque<PrInfixWS*> infixes;
  int postfix_n = 0;
};

struct PrInfixWS: Production {
  PrInfixWS(Token);
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const {return "";};
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  Token val;
};


struct PrDecor: Production {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const {return "";};
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext bc);
  
  PrDecor(Token rawToken);
  Token rawToken;
};

struct PrExpression: Production {
};

// parentheses are important to remember for beautifier
struct PrExprParen: PrExpression {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext bc);
  
  PrExpression* content;
};

struct PrExpressionFn: PrExpression{
  PrExpressionFn(Token identifier);
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  Token identifier;
  std::vector<PrExpression*> args;
};

struct PrExprArithmetic: PrExpression {
  PrExprArithmetic(PrExpression* lhs, Token op, PrExpression* rhs);
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* lhs;
  PrExpression *rhs;
  Token op;
};

struct PrStatement: Production {
  LBString end_statement_beautiful(const BeautifulConfig&, BeautifulContext);
};

struct PrEmptyStatement: PrStatement {
  PrEmptyStatement();
  PrEmptyStatement(Token enx);
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  bool hastoken;
  Token enx;
};

struct PrFinal: PrExpression {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  PrFinal(Token t);
  
  Token final;
};

struct PrIdentifier: PrExpression {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrIdentifier(Token t);
  
  Token identifier;
};

struct PrAssignment: PrStatement {
  PrAssignment(PrExpression* lhs, Token op, PrExpression* rhs);
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* lhs;
  PrExpression* rhs;
  Token op;
};

struct PrStatementFn: PrStatement {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpressionFn* fn;
};

struct PrVarDeclaration: Production {
  PrVarDeclaration(Token ident,  PrExpression* def = nullptr);
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  Token identifier;
  PrExpression* definition;
};

struct PrStatementVar: PrStatement {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  std::string type;
  std::vector<PrVarDeclaration*> declarations;
};

struct PrBody: PrStatement {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  bool is_root = false;
  
  std::vector<Production*> productions;
};

struct PrStatementIf: PrStatement {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* condition;
  PrStatement* result;
  PrStatement* otherwise=nullptr;
};

struct PrFor: PrStatement {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrStatement* init;
  PrExpression* condition;
  PrStatement* second;
  PrStatement* first;
};

struct PrWhile: PrStatement {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* condition;
  PrStatement* event;
};

struct PrRepeat: PrStatement {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* count;
  PrStatement* event;
};

struct PrDo: PrStatement {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* condition;
  PrStatement* event;
};

struct PrWith: PrStatement {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* objid;
  PrStatement* event;
};

struct PrAccessorExpression: PrExpression {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  std::string acc = "";
  PrExpression* ds;
  std::vector<PrExpression*> indices;
};

struct PrCase;

struct PrSwitch: PrStatement {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* condition;
  std::vector<PrCase*> cases;
};

struct PrCase: PrStatement {
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* value;
  std::vector<Production*> productions;
};

struct PrControl: PrStatement {
  PrControl(Token,PrExpression* val = nullptr);
  virtual std::string to_string();
  virtual LBString compile(CompilerContext& cc) const;
  virtual LBString beautiful(const BeautifulConfig&, BeautifulContext);
  
  Token kw;
  PrExpression* val;
};

#endif /*PRODUCTION_H*/