#include <vector>
#include <string>
#include <typeinfo>
#include <deque>

#include "tokenstream.h"

#ifndef BEAUTIFY_H
#define BEAUTIFY_H

struct BeautifulConfig {
  //! put open brace on same line
  bool egyptian = false;
  
  //! put semicolons at the end of each line
  bool semicolons = true;
  
  //! indent using spaces
  bool indent_spaces = true;
  
  //! number of spaces to indent with
  int indent_spaces_per_tab = 4;
  
  //! column width (-1 means arbitrary)
  int columns = -1;
  
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
  
  //! do not start a line with a binary operator (+-/* etc.)
  bool op_end_line = true;
  
// these options can modify the (non-comment/ws) tokens:

  //! parens for if statement condition (-1: leave untouched)
  int if_parens = -1;
  
  //! parens for while loop condition
  int while_parens = 1;

  //! parens for object id in with statement
  int with_parens = 0;
};

struct BeautifulContext {
 //! do not indent
  bool condense = false;
  
  //! block is attached to if/while/with/etc.
  bool attached = false;
  
  //! i.e. for for statements
  bool forced_semicolon = false;
  
  //! empty statements do not end with a semicolon
  bool never_semicolon = false;
  
  //! statement must not end with any blank lines (from postfixes)
  //! used at the end of blocks
  bool no_trailing_blanks = false;
  
  // 0: not eol, 1: eol, 2: internal eol
  char eol = 0;
  
  BeautifulContext as_eol() const;
  BeautifulContext not_eol() const;
  BeautifulContext as_internal_eol() const;
  BeautifulContext trim_leading_blanks() const;
  BeautifulContext force_semicolon() const;
};

const BeautifulContext DEFAULT_CONTEXT;

class Parser;
class PrInfixWS;

enum LBTreeType {
  LIST,
  CHUNK,
  PAD,
  NOPAD,
  FORCE
};

//! string with linebreaking information
class LBString {
  LBTreeType type;
  std::vector<LBString> list;
  std::string chunk;
  
  //! true if this is a linebreak
  bool taken = false;
  
public:
  LBString(LBTreeType type = LIST);
  LBString(std::string chunk);
  LBString(const char* chunk);
  LBString(const LBString& other);
  
  void operator+=(const LBString&);
  void append(LBString);
  void extend(const LBString&, bool append = false);
  
  void arrange(const BeautifulConfig&, int indent);
  std::string to_string(const BeautifulConfig&, int indent = 0, bool mark_nest = false);
private:
  std::string get_indent_string(const BeautifulConfig&, int indent = 0);
};

LBString operator+(const LBString&, const LBString&);
LBString operator+(const std::string, const LBString&);
LBString operator+(const LBString&, const std::string);
LBString operator+(const char*, const LBString&);
LBString operator+(const LBString&, const char*);
  
#endif /*BEAUTIFY_H*/