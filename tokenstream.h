#include <iostream>
#include <string>
#include <istream>
#include <queue>

enum TokenType {
  PUNC,
  OP,
  OPR,
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
  "PUNC",
  "OP",
  "OPR",
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
  Token(const TokenType type, const std::string valuet);
};

std::ostream &operator<<(std::ostream &,const Token &);

class TokenStream {
public:
  TokenStream(std::istream*);
  virtual Token read();
  Token peek();
  virtual bool eof();
private:
  Token next;
  std::istream* is;
  
  Token read_next();
  Token read_string();
  Token read_number();
  Token read_comment();
  Token read_comment_multiline();
  Token read_operator();
  Token read_ident();
  
  bool is_op_char(const unsigned char);
  bool is_punc_char(const unsigned char);
};

class LLKTokenStream: TokenStream {
public:
  LLKTokenStream(std::istream*, const int k);
  
  Token peek();
  Token peek(unsigned int skip);
  Token read();
  bool eof();
  bool has(unsigned int k);

private: 
  const int k;
  std::deque<Token> buffer;
};
