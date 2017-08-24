#include <cctype>

#include "tokenstream.h"

using namespace std;

Token::Token(const TokenType type, const std::string value): type(type), value(value) {
}

std::ostream &operator<<(std::ostream &os,const Token &token) {
  if (token.type == WS)
    return os << "--";
  if (token.type == END)
    return os << "EOF";
  return os << TOKEN_NAME[token.type] << ": " << token.value;
}

TokenStream::TokenStream(istream* istream): is(istream), next(END,"")  {
  read();
}

Token TokenStream::read_string() {
  unsigned char c;
  unsigned char terminal;
  string val;
  *is >> terminal; // determine terminal character
  if (is->eof())
    return Token(ERR,"Unterminated string");
  while (true) {
    c = is->get();
    if (c == terminal) break;
    if (is->eof())
      return Token(ERR,"Unterminated string");    
    if (c == '\\')
      c = is->get();
    if (is->eof())
      return Token(ERR,"Unterminated string");
    val += c;
  }
  return Token(STR,val);
}

Token TokenStream::read_number() {
  unsigned char c;
  string val;
  val += "";
  bool encountered_dot = false;;
  int position = 0;
  while (true) {
    if (is->eof())
      break;
    c = is->get();
    if (position == 0 && c == '#') {
      val += c;
      continue;
    }
    position += 1;
    if (c >= '0' && c <= '9') {
      val += c;
      continue;
    }
    if (c == '.' &&! encountered_dot) {
      val += c;
      encountered_dot = true;
      continue;
    }
    is->putback(c);
    break;
  }
  return Token(NUM, val);
}

Token TokenStream::read_comment() {
  string val;
  char c;
  c = is->get();
  c = is->get();
  val = "//";
  
  while (true) {
    if (is->eof())
      break;
    c = is->get();
    if (c == '\n')
      break;
    val += c;
  }
  
  return Token(COMMENT,val);
}

Token TokenStream::read_comment_multiline() {
  string val;
  char c;
  c = is->get();
  c = is->get();
  val = "/*";
  while (true) {
    c = is->get();
    if (c == '\n' || is->eof()) break;
    if (c == '*') {
      char c2;
      c2 = is->get();
      if (c2 == '/') {
        val += "*/";
        break;
      } else 
        is->putback(c2);
    }
    val += c;
  }
  
  return Token(COMMENT, val);
}

const char* op_multichar[] = {
  "++",
  "--",
  "+=",
  "!=",
  ">=",
  "<=",
  "||",
  "&&",
  "!=",
  "+=",
  "%=",
  "^=",
  "-=",
  "/=",
  "&=",
  "|=",
  "*=",
};

Token TokenStream::read_operator() {
  char c1;
  c1 = is->get();
  if (!is->eof()) {
    char c2;
    c2 = is->get();
    
    string multi(&c1);
    multi += c2;
    
    for (int i=0;i < sizeof(op_multichar)/sizeof(char*);i++)
      if (multi == op_multichar[i])
        return Token(OP,multi);
    
    is->putback(c2);
  }
  
  return Token(OP,string(1, (char)c1));
}


const char* KEYWORDS[] = {
  "if",
  "else",
  "while",
  "for",
  "var",
  "return",
  "exit"
};

Token TokenStream::read_ident() {
  int index = -1;
  unsigned char c;
  string val;
  while (true) {
    index += 1;
    if (is->eof())
      break;
    c = is->get();
    if ((c >= '0' && c <= '9') && index > 0) {
      val += c;
      continue;
    }
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
      val += c;
      continue;
    }
    is->putback(c);
    break;
  }
  if (val.length() == 0)
    return Token(ERR,"");
  for (int i = 0; i< sizeof(KEYWORDS)/sizeof(char*);i+=1)
    if (val == KEYWORDS[i])
      return Token(KW,val);
  return Token(ID,val);
}

Token TokenStream::read_next() {
  unsigned char in;
  if (is->eof())
    return Token(END,"");
  // read whitespace:
  while (true) {
    in = is->get();
    if (!isspace(in) || is->eof()) break;
  }
  if (is->eof())
    return Token(END,"");
  // parse token:
  if (in == '"' || in == '\'') {
    is->putback(in);
    return read_string();
  }
  if (in >= '0' && in <= '9') {
    is->putback(in);
    return read_number();
  }
  if ((in == '.' || in == '#') &&! is->eof()) {
    unsigned char in2;
    in2 = is->get();
    is->putback(in2);
    if (in2 >= '0' && in2 <= '9') {
      return read_number();
    }
  }
  if (in == '/' &&! is->eof()) {
    unsigned char in2;
    in2 = is->get();
    is->putback(in2);
    if (in2 == '/') {
      is->putback(in);
      return read_comment();
    }
    if (in2 == '*') {
      is->putback(in);
      return read_comment_multiline();
    }
  }
  if (is_punc_char(in))
    return Token(PUNC,string(1, (char)in));
  if (is_op_char(in)) {
    is->putback(in);
    return read_operator();
  }
  is->putback(in);
  return read_ident();
}

Token TokenStream::read() {
  Token to_return = next;
  next = read_next();
  return to_return;
}

Token TokenStream::peek() {
  return next;
}

bool TokenStream::eof() {
  return peek().type == END || peek().type == ERR;
}

const char ops[] = "+-/*?<>=!%|&^";
const char punc[] = "(){}.,;[]:";

bool TokenStream::is_op_char(const unsigned char c) {
  for (int i=0;i<sizeof(ops);i++) {
    if (ops[i] == c)
      return true;
  }
  return false;
}

bool TokenStream::is_punc_char(const unsigned char c) {
  for (int i=0;i<sizeof(ops);i++) {
    if (punc[i] == c)
      return true;
  }
  return false;
}
