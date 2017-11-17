#include <iostream>
#include <string>
#include <istream>
#include <queue>

#ifndef TOKENSTREAM_H
#define TOKENSTREAM_H

enum TokenType {
  PUNC,
  OP,
  OPR,
  OPA,
  NUM,
  STR,
  KW,
  ID,
  COMMENT,
  WS,
  ENX,
  END,
  ERR
};

static const char* TOKEN_NAME[] = {
  "PUNC", // (),. etc.
  "OP", // operator
  "OPR", //++ or --
  "OPA", //accessor operator
  "NUM",
  "STR",
  "KW",
  "ID",
  "COMMENT",
  "WS",
  "ENX",
  "END",
  "ERR"
};

struct Token {
  TokenType type;
  std::string value;
  Token();
  Token(const TokenType type, const std::string value);
  bool operator==(const Token& other) const;
  bool operator!=(const Token& other) const;
  bool is_op_keyword();
};

std::ostream &operator<<(std::ostream &,const Token &);

class TokenStream {
public:
  TokenStream(std::istream*);
  TokenStream(std::string);
  ~TokenStream();

  // gobbles the next token, returning it
  virtual Token read();
  
  // peeks at next token but does not gobble it
  Token peek() const;
  
  // returns (row, column) pair of where the lexer currently is in the input
  std::pair<int,int> location() const;
  
  // end of file has been reached; peek() or read() will fail.
  virtual bool eof();
private:
  Token next;
  std::istream* is;
  bool istream_mine;
  unsigned int row=1;
  unsigned int col=0;
  
  char read_char();
  
  Token read_next();
  Token read_string();
  Token read_number();
  Token read_comment();
  Token read_comment_multiline();
  Token read_operator();
  Token read_ident();
  
  bool is_op_char(const unsigned char);
  bool is_opa_char(const unsigned char);
  bool is_punc_char(const unsigned char);
};

class LLKTokenStream: TokenStream {
public:
  LLKTokenStream(std::istream*, const int k);
  LLKTokenStream(std::string, const int k);
  
  Token peek();
  Token peek(unsigned int skip);
  std::pair<int,int> location() const;
  Token read();
  bool eof();
  bool has(unsigned int k);

private: 
  const int k;
  std::deque<Token> buffer;
};

#endif /* TOKENSTREAM_H */
