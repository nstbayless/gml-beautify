#include <vector>
#include <string>
#include <typeinfo>
#include <deque>

#include "tokenstream.h"

#ifndef PARSER_H
#define PARSER_H

enum InfixStyle {
  //! leave as-is
  AS_IS,
  
  /** the following are not styles per se but do modify flags if passed to .style() */
  PAD_LEFT,
  PAD_RIGHT,
  PAD_BOTH,
  PAD_NEITHER
};

struct BeautifulConfig {
  //! put open brace on same line
  bool egyptian = true;
  
  //! put semicolons at the end of each line
  bool semicolons = true;
  
  //! indent using spaces
  bool indent_spaces = true;
  
  //! number of spaces to indent with
  int indent_spaces_per_tab = 4;
  
  //! remove blank lines from start and end of blocks {}
  bool trim_block = true;
  
  //! put spacing around ++/-- operator
  bool opr_space = false;
  
  //! put spacing after ! and ~ operators
  bool not_space = false;
  
  //! put spacing after accessor symbol, e.g. array[@ index]
  bool accessor_space = true;
  
  //! if a comment starts a line but is not the first line of a body, it must follow a blank line.
  bool blank_before_comment = true;
  
// these options can modify the (non-comment/ws) tokens:

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
  
  //! i.e. for for statements
  bool forced_semicolon = false;
  
  //! empty statements do not end with a semicolon
  bool never_semicolon = false;
  
  //! statement must not end with any blank lines (from postfixes)
  //! used at the end of blocks
  bool no_trailing_blanks = false;
  
  InfixStyle infix_style = AS_IS;
  bool pad_infix_left = true;
  bool pad_infix_right = false;
  // 0: not eol, 1: eol, 2: internal eol
  char eol = 0;
  
  BeautifulContext increment_depth() const;
  BeautifulContext decrement_depth() const;
  BeautifulContext as_inline() const;
  BeautifulContext not_inline() const;
  BeautifulContext as_eol() const;
  BeautifulContext not_eol() const;
  BeautifulContext as_internal_eol() const;
  BeautifulContext trim_leading_blanks() const;
  
  // floating block control:
  BeautifulContext attach() const;
  BeautifulContext detach() const;
  
  BeautifulContext force_semicolon() const;
  BeautifulContext style(InfixStyle) const;
};

const BeautifulContext DEFAULT_CONTEXT;

class Parser;
class PrInfixWS;

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
