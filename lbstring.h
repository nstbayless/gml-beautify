#include "beautify.h"

#ifndef LBSTRING_H
#define LBSTRING_H

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
  bool contents_indented = false;
  float break_cost = 1;
  std::string chunk;
  
  //! true if this is a linebreak
  bool taken = false;
  
public:
  LBString(LBTreeType type = LIST, float cost = 1);
  LBString(std::string chunk);
  LBString(const char* chunk);
  LBString(const LBString& other);
  
  // indents string, and returns reference to self
  LBString& indent(bool do_indent = true);
  
  //! alias for extend
  void operator+=(const LBString&);
  void extend(const LBString&, bool append = false);
  void append(LBString);
  //! starts a new line if not already on one.
  void new_line();
  
  void arrange(const BeautifulConfig&, int indent);
  void trim(bool left = true, bool right = true);
  std::string to_string_unarranged(const BeautifulConfig&, int indent = 0, bool mark_nest = false) const;
  
  //! arranges and the returns string
  std::string to_string(const BeautifulConfig&, int indent = 0, bool mark_nest = false, bool ltrim=true, bool rtrim=true);
private:
  //! flattens list except for indented sub-lists after newline
  void flatten_to_indent();
  std::string get_indent_string(const BeautifulConfig&, int indent = 0) const;
  void arrange_sublist(const BeautifulConfig&, int indent, int start_index, int end_index);
};

LBString operator+(const LBString&, const LBString&);
LBString operator+(const std::string, const LBString&);
LBString operator+(const LBString&, const std::string);
LBString operator+(const char*, const LBString&);
LBString operator+(const LBString&, const char*);

#endif /*LBSTRING_H*/
