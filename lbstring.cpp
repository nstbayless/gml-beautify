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

void LBString::append(LBString other) {
  if (type == LIST) {
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
      
      // sub lists absorb left-whitespace into them and eject their own right-whitespace
      if (other.type == LIST) {
        if (other.list.empty())
          return;
        while (list.back().type >= PAD) {
          other.list.insert(other.list.begin() + 0,list.back());
          list.pop_back();
          if (list.empty())
            break;
        }
        list.push_back(other);
        auto& ll = list.back();
        int k = 0;
        while (ll.list.back().type >= PAD) {
          list.insert(list.begin() + (list.size() + (k--)), ll.list.back());
          ll.list.pop_back();
          if (ll.list.empty())
            break;
        }
        return;
      }
    }
    list.push_back(other);
  } else {
    // not supported
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
      // not supported
    }
  }
}

void LBString::arrange(const BeautifulConfig& config, int indent) {
  if (config.columns < 0)
    return;
}

std::string LBString::to_string(const BeautifulConfig& config, int indent, bool mark_nesting) {
  arrange(config, indent);
  switch (type) {
    case LIST: {
      std::string s = "";
      if (mark_nesting)
        s += ":[";
      for (auto& iter: list) {
        s += iter.to_string(config, indent + 1, mark_nesting);
      }
      if (mark_nesting)
        s += "]:";
      return s;
    }
    case CHUNK:
      return chunk;
    case PAD: 
      if (!taken) return " ";
    case NOPAD:
      if (!taken) return "";
    case FORCE:
      return "\n" + get_indent_string(config, indent);
  }
}

std::string LBString::get_indent_string(const BeautifulConfig& config, int indent) {
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