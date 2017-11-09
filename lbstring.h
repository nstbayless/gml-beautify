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
  std::string chunk;
  
  //! true if this is a linebreak
  bool taken = false;
  
public:
  LBString(LBTreeType type = LIST);
  LBString(std::string chunk);
  LBString(const char* chunk);
  LBString(const LBString& other);
  
  LBString& indent(bool do_indent = true);
  
  void operator+=(const LBString&);
  void append(LBString);
  void extend(const LBString&, bool append = false);
  
  void arrange(const BeautifulConfig&, int indent);
  std::string to_string_unarranged(const BeautifulConfig&, int indent = 0, bool mark_nest = false) const;
  
  // arranges and the returns string
  std::string to_string(const BeautifulConfig&, int indent = 0, bool mark_nest = false);
private:
  std::string get_indent_string(const BeautifulConfig&, int indent = 0) const;
  void arrange_sublist(const BeautifulConfig&, int indent, int start_index, int end_index);
};

LBString operator+(const LBString&, const LBString&);
LBString operator+(const std::string, const LBString&);
LBString operator+(const LBString&, const std::string);
LBString operator+(const char*, const LBString&);
LBString operator+(const LBString&, const char*);

#endif /*LBSTRING_H*/