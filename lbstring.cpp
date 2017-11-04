#include "beautify.h"

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
        if (other.chunk.front() == ' ' && chunk.back() == ' ')
          chunk.pop_back();
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

void LBString::arrange_sublist(const BeautifulConfig& config, int indent, int start, int end) {
  // gather data on what break points are available
  std::vector<int> break_indices;
  std::vector<unsigned int> chunk_lengths;
  unsigned int cc = 0;
  int max_width = config.columns - indent * config.indent_spaces_per_tab;
  for (int i=start;i<end;i++) {
    if (list[i].type == CHUNK)
      cc += list[i].chunk.length();
    if (list[i].type == PAD)
      cc ++;
    if (list[i].type >= PAD) {
      break_indices.push_back(i);
      chunk_lengths.push_back(cc);
      cc = 0;
    }
  }
  
  break_indices.push_back(-1);
  chunk_lengths.push_back(cc);
  
  // assign pads taken
  int acc_width = 0;
  int row_start_length = config.indent_spaces_per_tab * config.premature_linebreak_indent;
  for (int i=0;i<break_indices.size();i++) {
    if (acc_width > row_start_length) {
      if (chunk_lengths[i] + acc_width > max_width) {
        acc_width = row_start_length;
        list[break_indices[i - 1]].taken = true;
      }
    }
    
    acc_width += chunk_lengths[i];
  }
}

std::string LBString::to_string(const BeautifulConfig& config, int indent, bool mark_nesting) { 
  arrange(config, indent);
  return to_string_unarranged(config, indent, mark_nesting);
}

std::string LBString::to_string_unarranged(const BeautifulConfig& config, int indent, bool mark_nesting) const {
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
      if (!taken) return " ";
    case NOPAD:
      if (!taken) return "";
      return "\n" + get_indent_string(config, indent + config.premature_linebreak_indent);
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