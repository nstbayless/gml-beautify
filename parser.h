#include <vector>
#include <string>
#include <typeinfo>
#include <deque>

#include "tokenstream.h"
#include "beautify.h"

#ifndef PARSER_H
#define PARSER_H

struct Production {
  friend class Parser;
  virtual ~Production();
  
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&,
    BeautifulContext bc = DEFAULT_CONTEXT);
  
  std::string renderWS(const BeautifulConfig&,
    BeautifulContext bc);
  
  std::string renderPostfixesTrimmed(const BeautifulConfig&,
    BeautifulContext bc);
  
  void flattenPostfixes();
  
  std::deque<PrInfixWS*> infixes;
  int postfix_n = 0;
};

struct PrInfixWS: Production {
  PrInfixWS(Token);
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  Token val;
  InfixStyle style = AS_IS;
};


struct PrDecor: Production {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext bc);
  
  PrDecor(Token rawToken);
  
  Token rawToken;
};

struct PrExpression: Production {
};

// parentheses are important to remember for beautifier
struct PrExprParen: PrExpression {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext bc);
  
  PrExpression* content;
};

struct PrExpressionFn: PrExpression{
  PrExpressionFn(Token identifier);
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  Token identifier;
  std::vector<PrExpression*> args;
};

struct PrExprArithmetic: PrExpression {
  PrExprArithmetic(PrExpression* lhs, Token op, PrExpression* rhs);
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* lhs;
  PrExpression *rhs;
  Token op;
};

struct PrStatement: Production {
  std::string end_statement_beautiful(const BeautifulConfig&, BeautifulContext);
};

struct PrEmptyStatement: PrStatement {
  PrEmptyStatement();
  PrEmptyStatement(Token enx);
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  bool hastoken;
  Token enx;
};

struct PrFinal: PrExpression {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  PrFinal(Token t);
  
  Token final;
};

struct PrIdentifier: PrExpression {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrIdentifier(Token t);
  
  Token identifier;
};

struct PrAssignment: PrStatement {
  PrAssignment(PrExpression* lhs, Token op, PrExpression* rhs);
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* lhs;
  PrExpression* rhs;
  Token op;
};

struct PrStatementFn: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpressionFn* fn;
};

struct PrVarDeclaration: Production {
  PrVarDeclaration(Token ident,  PrExpression* def = nullptr);
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  Token identifier;
  PrExpression* definition;
};

struct PrStatementVar: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  std::vector<PrVarDeclaration*> declarations;
};

struct PrBody: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  bool is_root = false;
  
  std::vector<Production*> productions;
};

struct PrStatementIf: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* condition;
  PrStatement* result;
  PrStatement* otherwise=nullptr;
};

struct PrFor: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrStatement* init;
  PrExpression* condition;
  PrStatement* second;
  PrStatement* first;
};

struct PrWhile: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* condition;
  PrStatement* event;
};

struct PrWith: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* objid;
  PrStatement* event;
};

struct PrAccessorExpression: PrExpression {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  std::string acc = "";
  PrExpression* ds;
  std::vector<PrExpression*> indices;
};


struct PrCase: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* value;
  std::vector<Production*> productions;
};

struct PrSwitch: PrStatement {
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  PrExpression* condition;
  std::vector<PrCase*> cases;
};

struct PrControl: PrStatement {
  PrControl(Token,PrExpression* val = nullptr);
  virtual std::string to_string();
  virtual std::string beautiful(const BeautifulConfig&, BeautifulContext);
  
  Token kw;
  PrExpression* val;
};

class Parser {
public:
  Parser(std::istream* is);
  
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
  PrSwitch* read_switch();
  
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
