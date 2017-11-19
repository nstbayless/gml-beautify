#include "beautify.h"
#include "lbstring.h"
#include "util.h"

LBString::LBString(LBTreeType type): type(type) {
  if (type == FORCE)
    taken = true;
}

LBString::LBString(std::string chunk): type(CHUNK), chunk(chunk) {};
LBString::LBString(const char* chunk): type(CHUNK), chunk(chunk) {};

LBString::LBString(const LBString& other) {
  type = other.type;
  list = other.list;
  chunk = other.chunk;
  contents_indented = other.contents_indented;
  taken = other.taken;
}

void LBString::operator+=(const LBString& other) {
  extend(other);
}

LBString operator+(const LBString& a, const LBString& b) {
  LBString lbs;
  lbs += a;
  lbs += b;
  return lbs;
}

LBString operator+(const std::string a, const LBString& b) {
  return LBString(a) + b;
}

LBString operator+(const LBString& a, const std::string b) {
  return a + LBString(b);
}

LBString operator+(const char* a, const LBString& b) {
  return LBString(a) + b;
}

LBString operator+(const LBString& a, const char* b) {
  return a + LBString(b);
}

LBString& LBString::indent(bool t) {
  contents_indented = t;
  return *this;
}

void LBString::append(LBString other) {
  if (type == LIST) {
    // empty chunks are not allowed in lists.which reminds me, the engine really does need a name...
    if (other.type == CHUNK) {
      if (other.chunk.length() == 0)
        return;
    }
    if (!list.empty()) {
      LBTreeType back = list.back().type;
      
      // glue chunks together
      if (other.type == CHUNK && back == CHUNK) {
        if (other.chunk == "")
          return;
        else if (list.back().chunk != "")
          if (other.chunk.front() == ' ' && list.back().chunk.back() == ' ')
            list.back().chunk.pop_back();
        other.chunk = list.back().chunk + other.chunk;
        list.pop_back();
      }
      
      // consecutive no-pads
      if (other.type == NOPAD && back == NOPAD) {
        return;
      }
      
      // consecutive pads
      if ((other.type == PAD || other.type == NOPAD) && (back == PAD  || back == NOPAD)) {
        list.pop_back();
        list.push_back(LBString(PAD));
        return;
      } 
      
      // consecutive forces
      if (other.type == FORCE && back == FORCE) {
        list.push_back(other);
        return;
      }
       
      // force adjacent to pad
      if ((other.type == FORCE && back >= PAD) || (other.type >= PAD && back == FORCE)) {
        list.pop_back();
        list.push_back(LBString(FORCE));
        return;
      }
                       
      // pad and then space
      if (other.type == CHUNK && back >= PAD) {
        if (other.chunk.front() == ' ')
          other.chunk = other.chunk.substr(1,other.chunk.size() - 1);
      }
      
      // space and then pad
      if (back == CHUNK && other.type >= PAD) {
        if (list.back().chunk.back() == ' ') {
          list.back().chunk.pop_back();
        }
      }
      
      // indented sub-lists absorb left-whitespace into them and eject their own right-whitespace
      if (other.type == LIST && other.contents_indented) {
        if (other.list.empty())
          return;
        
        // donate own right-whitespace to new sublist (to other)
        while (list.back().type >= PAD) {
          other.list.insert(other.list.begin() + 0,list.back());
          list.pop_back();
          if (list.empty())
            break;
        }
        
        // append new sublist (to other)
        list.push_back(other);
        
        // siphon out new sublist's right-whitespace
        int lln = list.size() - 1;
        int k = 0;
        while (list[lln].list.back().type >= PAD) {
          list.insert(list.begin() + (list.size() + (k--)), list[lln].list.back());
          list[lln].list.pop_back();
          if (list[lln].list.empty())
            break;
        }
        return;
      }
    }
    list.push_back(other);
  } else {
    LBString altme = *this;
    type = LIST;
    chunk = "";
    list.push_back(altme);
    append(other);
  }
}

void LBString::extend(const LBString& other, bool do_append) {  
  if (do_append) {
    append(other);
  } else {
    // extend
    if (other.type == LIST && type == LIST) {
      for (int i=0;i<other.list.size();i++)
        append(other.list[i]);
    } else if (type == LIST) {
      append(other);
    } else {
      LBString altme = *this;
      type = LIST;
      chunk = "";
      list.push_back(altme);
      extend(other);
    }
  }
}

void LBString::new_line() {
  if (type != LIST) {
    if (type != FORCE)
      extend(FORCE);
  } else if (list.back().type != FORCE) {
    if (list.back().type == LIST)
      list.back().new_line();
    else
      extend(FORCE);
  }
}

void LBString::arrange(const BeautifulConfig& config, int indent) {
  indent += contents_indented;
  if (config.columns < 0)
    return;
  if (type != LIST)
    return;
  
  
  // only lists need arranging
  // find indexes of all forced breaks, arrange intervening portions separately
  int previous_break = 0;
  for (int i=0;i<list.size();i++) {
    if (list[i].type == FORCE || list[i].type == LIST) {
      arrange_sublist(config, indent, previous_break, i);
      if (list[i].type == LIST) {
        list[i].arrange(config, indent);
      }
      previous_break = i+1;
    }
  }
  
   arrange_sublist(config, indent, previous_break, list.size());
}

void LBString::flatten_to_indent() {
  for (int i=0;i<list.size();i++) {
    if (list[i].type == LIST) {
      if (list[i].list.empty())
        list.erase(list.begin() + i);
      else {
        // pop element from front
        if (!list[i].contents_indented || list[i].list.front().type != FORCE) {
          list.insert(list.begin()+i, list[i].list.front());
          list[i+1].list.erase(list.begin());
        }
      }
    }
  }
}

void LBString::arrange_sublist(const BeautifulConfig& config, int indent, int start, int end) {
  std::vector<float> opt_break_costs;
  std::vector<int> opt_next_break;
  opt_break_costs.push_back(0);
  opt_next_break.push_back(0);
  
  // gather data on what break points are available
  std::vector<int> break_indices;
  std::vector<unsigned int> chunk_lengths;
  break_indices.push_back(0);
  chunk_lengths.push_back(0);

  unsigned int cc = 0;
  for (int i=start;i<end;i++) {
    if (list[i].type == CHUNK)
      cc += list[i].chunk.length();
    if (list[i].type == PAD)
      cc ++;
    if (list[i].type >= PAD) {
      break_indices.push_back(i);
      chunk_lengths.push_back(cc);
      opt_break_costs.push_back(list[i].break_cost);
      opt_next_break.push_back(i+1);
      cc = 0;
    }
  }
  
  break_indices.push_back(-1);
  chunk_lengths.push_back(cc);
  opt_break_costs.push_back(0);
  opt_next_break.push_back(-1);
  int max_width_first = config.columns - indent * config.indent_spaces_per_tab;
  int max_width_etc = max_width_first - config.indent_spaces_per_tab * config.premature_linebreak_indent;
  
  // DP solution for linebreaking
  for (int i = break_indices.size()-2; i>=0; i--) {
    int min_next_id = i+1;
    int nonbreak_width = 0;
    int max_width = (i == 0)? max_width_first : max_width_etc;
    // scan over next breakpoints and pick minimum
    for (int j = i + 1; j < break_indices.size();j++) {
      // make sure we don't go beyond chunk size
      nonbreak_width += chunk_lengths[j];
      if (nonbreak_width > max_width)
        break;
      
      // check how this breakpoint compares as next:
      if (opt_break_costs[j] <= opt_break_costs[min_next_id])
        min_next_id = j;
    }
    opt_break_costs[i] += opt_break_costs[min_next_id];
    opt_next_break[i] = min_next_id;
  }
  
  // follow DP solution:
  for (int s=opt_next_break[0];opt_next_break[s] != -1;s = opt_next_break[s]) {
    list[break_indices[s]].taken = true;
  }
}

std::string LBString::to_string(const BeautifulConfig& config, int indent, bool mark_nesting, bool ltrim, bool rtrim) { 
  trim(ltrim, rtrim);
  arrange(config, indent);
  return to_string_unarranged(config, indent, mark_nesting);
}

void LBString::trim(bool left, bool right) {
  switch (type) {
    case LIST:
      if (left) {
        while (!list.empty()) {
          if (list.front().type >= PAD)
            list.erase(list.begin());
          else {
            list.front().trim(true, false);
            if (list.front().type == LIST) {
              if (list.front().list.size() == 0) {
                list.erase(list.begin());
                continue;
              }
            }
            if (list.front().type == CHUNK) {
              if (list.front().chunk.size() == 0) {
                list.erase(list.begin());
                continue;
              }
            }
            break;
          }
        }
      }
      if (right) {
        while (!list.empty()) {
          if (list.back().type >= PAD)
            list.pop_back();
          else {
            list.back().trim(false, true);
            if (list.back().type == LIST) {
              if (list.back().list.size() == 0) {
                list.pop_back();
                continue;
              }
            }
            if (list.back().type == CHUNK) {
              if (list.back().chunk.size() == 0) {
                list.pop_back();
                continue;
              }
            }
            break;
          }
        }
      }
      break;
    case CHUNK:
      if (left) {
        ltrim(chunk);
      }
      if (right) {
        rtrim(chunk);
      }
  }
}

std::string LBString::to_string_unarranged(const BeautifulConfig& config, int indent, bool mark_nesting) const {
  std::string mts = "";
  if (mark_nesting)
    mts = "^";
  switch (type) {
    case LIST: {
      std::string s = "";
      if (mark_nesting) {
        if (contents_indented)
          s += "=[";
        else
          s += ":[";
      }
      for (auto& iter: list) {
        s += iter.to_string_unarranged(config, indent + contents_indented, mark_nesting);
      }
      if (mark_nesting) {
        if (contents_indented)
          s += "]=";
        else
          s += "]:";
      }
      return s;
    }
    case CHUNK:
      return chunk;
    case PAD: 
      if (!taken) return mts + " ";
    case NOPAD:
      if (!taken) return mts;
      return mts + "\n" + get_indent_string(config, indent + config.premature_linebreak_indent);
    case FORCE:
      return "\n" + get_indent_string(config, indent);
  }
}

std::string LBString::get_indent_string(const BeautifulConfig& config, int indent) const {
  std::string s = "";
  std::string tab = "\t";
  if (config.indent_spaces) {
    tab = "";
    for (int i=0; i < config.indent_spaces_per_tab;i++)
      tab += " ";
  }
  for (int i=0;i<indent;i++) {
    s += tab;
  }
  return s;
}
