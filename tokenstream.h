#include <iostream>
#include <string>
#include <istream>

enum TokenType {
  PUNC,
  OP,
  NUM,
  STR,
  KW,
  ID,
  COMMENT,
  WS,
  END,
  ERR
};

static const char* TOKEN_NAME[] = {
  "PUNC",
  "OP",
  "NUM",
  "STR",
  "KW",
  "ID",
  "COMMENT",
  "WS",
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
  Token read();
  Token peek();
  bool eof();
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
