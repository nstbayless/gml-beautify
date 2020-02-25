#include <cctype>
#include <sstream>

#include "tokenstream.h"
#include "util.h"

using namespace std;

Token::Token(const TokenType type, const std::string value):
  type(type),
  value(value)
{ }

Token::Token(): Token(ERR,"")
{ }

bool Token::operator==(const Token& other) const {
  return type == other.type && value == other.value;
}

bool Token::operator!=(const Token& other) const {
  return !(*this == other);
}

bool Token::is_op_keyword() {
  if (type == KW) {
    if (value == "not")
      return true;
    if (value == "and")
      return true;
    if (value == "or")
      return true;
if (value == "xor")
      return true;
    if (value == "mod")
      return true;
    if (value == "div")
      return true;
  }
  return false;
}

std::ostream &operator<<(std::ostream &os,const Token &token) {
  if (token.type == WS)
    return os << "--";
  if (token.type == END)
    return os << "EOF";
  return os << TOKEN_NAME[token.type] << ": " << token.value;
}

TokenStream::TokenStream(istream* istream): is(istream), next(END,""), istream_mine(false) {
  read();
}

std::pair<int,int> TokenStream::location() const {
  return std::pair<int, int>(row, col);
}

TokenStream::TokenStream(std::string s): is(new std::stringstream(s)), next(END,""), istream_mine(true) {
  read();
}

TokenStream::~TokenStream() {
  if (istream_mine)
    delete(is);
}
  
char TokenStream::read_char() {
  char c = is->get();
  if (c=='\n') {
    col = 0;
    row ++;
  } else {
    col ++;
  }
  return c;
}

Token TokenStream::read_string() {
  unsigned char c;
  unsigned char terminal;
  string val;
  *is >> terminal; // determine terminal character
  if (is->eof())
    return Token(ERR,"Unterminated string");
  while (true) {
    c = read_char();
    if (c == terminal) break;
    if (is->eof())
      return Token(ERR,"Unterminated string");    
    val += c;
  }
  std::string terminal_str = " ";
  terminal_str[0] = terminal;
  return Token(STR,terminal_str + val + terminal_str);
}

Token TokenStream::read_number(bool hex) {
  unsigned char c;
  string val;
  bool encountered_dot = false;
  int position = 0;
  if (hex)
    val += read_char(); //$
  while (true) {
    if (is->eof())
      break;
    c = read_char();
    position += 1;
    if ((c >= '0' && c <= '9') || (hex && ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')))) {
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
  c = read_char();
  c = read_char();
  val = "//";
  
  while (true) {
    if (is->eof())
      break;
    c = read_char();
    if (c == '\n' || c == -1) {
      is->putback(c);
      break;
    }
    val += c;
  }
  
  return Token(COMMENT,val);
}

Token TokenStream::read_comment_multiline() {
  string val;
  char c;
  c = read_char();
  c = read_char();
  val = "/*";
  while (true) {
    if (is->eof()) break;
    c = read_char();
    if (c == -1 || is->eof()) {
      rtrim(val);
      break;
    }
    if (c == '*') {
      char c2;
      c2 = read_char();
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
  "==",
  "||",
  "&&",
  "^^",
  "!=",
  "+=",
  "%=",
  "^=",
  "-=",
  "/=",
  "&=",
  "|=",
  "*=",
  "~="
  "<<",
  ">>",
  "<>"
};

Token TokenStream::read_operator() {
  char c1;
  c1 = read_char();
  if (is_opa_char(c1)) {
    return Token(OPA,string(1,(char)c1));
  }
  if (!is->eof()) {
    char c2;
    c2 = read_char();
    
    string multi(1, c1);
    multi += c2;
    
    for (int i=0;i < sizeof(op_multichar)/sizeof(char*);i++)
      if (multi == op_multichar[i])
        return Token((i <= 1)?OPR:OP,multi);
    
    is->putback(c2);
  }
  
  return Token(OP,string(1, (char)c1));
}


const char* KEYWORDS[] = {
  "var",
  "globvar",
  "if",
  "else",
  "while",
  "repeat",
  "do",
  "until",
  "with",
  "for",
  "switch",
  "case",
  "default",
  "break",
  "continue",
  "return",
  "exit",
  "not",
  "and",
  "or",
  "xor",
  "mod",
  "div",
};

Token TokenStream::read_ident() {
  int index = -1;
  unsigned char c;
  string val;
  while (true) {
    index += 1;
    if (is->eof())
      break;
    c = read_char();
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
    in = read_char();
    if (!isspace(in) || in == '\n'|| is->eof()) break;
  }
  if (in == '\n')
    return Token(ENX,string(1, (char)in));
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
  if ((in == '.' || in == '$') &&! is->eof()) {
    unsigned char in2;
    in2 = read_char();
    is->putback(in2);
    if ((in2 >= '0' && in2 <= '9') || in == '$') {
      is->putback(in);
      return read_number(in == '$');
    }
  }
  if (in == '/' &&! is->eof()) {
    unsigned char in2;
    in2 = read_char();
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
  if (in == ';')
    return Token(ENX,string(1, (char)in));
  if (is_punc_char(in))
    return Token(PUNC,string(1, (char)in));
  if (is_op_char(in) || is_opa_char(in)) {
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

Token TokenStream::peek() const {
  return next;
}

bool TokenStream::eof() {
  return peek().type == END || peek().type == ERR;
}

const char ops[] = "+-*/<>=&|!%^~";
const char opas[] = "?#@";
const char punc[] = "(){}.,[]:";

bool TokenStream::is_op_char(const unsigned char c) {
  for (int i=0;i<sizeof(ops);i++) {
    if (ops[i] == c)
      return true;
  }
  return false;
}

bool TokenStream::is_opa_char(const unsigned char c) {
  for (int i=0;i<sizeof(opas);i++) {
    if (opas[i] == c)
      return true;
  }
  return false;
}

bool TokenStream::is_punc_char(const
 unsigned char c) {
  for (int i=0;i<sizeof(ops);i++) {
    if (punc[i] == c)
      return true;
  }
  return false;
}

LLKTokenStream::LLKTokenStream(istream* is, int k): TokenStream(is), k(k) {
  while (buffer.size() < k - 1 && !TokenStream::eof())
    buffer.push_back(TokenStream::read());
}

LLKTokenStream::LLKTokenStream(std::string s, int k): TokenStream(s), k(k) {
  while (buffer.size() < k - 1 && !TokenStream::eof())
    buffer.push_back(TokenStream::read());
}

Token LLKTokenStream::peek() const {
  if (buffer.size() == 0)
    return TokenStream::peek();
  return buffer.front();
}

Token LLKTokenStream::peek(unsigned int i) const {
  if (i == k - 1)
    return TokenStream::peek();
  else
    return buffer[i];
}

std::pair<int,int> LLKTokenStream::location() const {
  return TokenStream::location();
}

Token LLKTokenStream::read() {
  if (!TokenStream::eof())
    buffer.push_back(TokenStream::read());
  Token to_return = buffer.front();
  buffer.pop_front();
  return to_return;
}

bool LLKTokenStream::eof() {
  return buffer.size() == 0;
}

bool LLKTokenStream::has(unsigned int k) {
  return buffer.size() > k;
}
