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
  
  //! column width (-1 means infinite, -2 means preserve line endings)
  int columns = -2;
  
  //! number of linebreaks if wrapping
  int premature_linebreak_indent = 1;
  
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
  
  //! comments start with a space // like this, not //like this
  bool comment_space = true;
  
  //! do not start a line with a binary operator (+-/* etc.)
  bool op_end_line = true;
  
// these options can modify the (non-comment/ws) tokens:
  
  // use == instead of =
  bool force_double_equals_comparison = true;
  
  // 0: leave as-is. 1: use !,&&,^^,||. 2: use not, and, xor, or
  char compare_style = 1;
  
  // 0: leave as-is. 1: if (blah). 2: if blah.
  char cond_parens = 1;
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
  
  //! do not render single newlines (disabled for postfixes)
  bool no_single_newline = true;
  
  //! linbreaking cost multiplier
  float cost_mult = 1;
  
  // 0: not eol, 1: eol, 2: internal eol
  char eol = 0;
  
  BeautifulContext as_eol() const;
  BeautifulContext not_eol() const;
  BeautifulContext as_internal_eol() const;
  BeautifulContext trim_leading_blanks() const;
  BeautifulContext force_semicolon() const;
};

const BeautifulContext DEFAULT_CONTEXT;
  
#endif /*BEAUTIFY_H*/
