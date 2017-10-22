#include "beautify.h"

LBString::LBString(LBTreeType type): type(type) {
  if (type == FORCE)
    taken = true;
}

LBString::LBString(std::string chunk): type(CHUNK), chunk(chunk) {};

void LBString::operator+=(const LBString& other) {
  append(other);
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

LBString operator+(const LBString& a, std::string b) {
  return a + LBString(b);
}

void LBString::append(const LBString& other) {
  if (type == LIST)
    list.push_back(other);
  else {
    // not supported
  }
}

void LBString::extend(const LBString& other) {
  // need to determine if back is chunk
  // to do chunk gluing.
  bool back_is_chunk = false;
  if (!list.empty()) {
    if (list.back().type == CHUNK) {
      back_is_chunk = true;
    }
  }
  
  // extend
  if (other.type == LIST && type == LIST) {
    for (int i=0;i<other.list.size();i++)
      if (!other.list.empty() && back_is_chunk) {
        auto& immediate = other.list[0]
        if (immediate.type == CHUNK) {
          // glue chunks together
          immediate.chunk = list.back().chunk + immediate.chunk;
          list.pop_back();
        }
      }
      list.push_back(other.list[i]);
  } else if (type == LIST) {
    if (back_is_chunk && other.type == CHUNK) {
      // glue chunks together
      list.back().chunk += other.chunk;
    } else {
      list.push_back(other);
    }
  } else {
    // not supported
  }
}

void LBString::arrange(const BeautifulConfig& config, int indent) {
  if (config.columns < 0)
    return;
}

std::string LBString::to_string(const BeautifulConfig& config, int indent) {
  arrange(config, indent);
  switch (type) {
    case LIST: {
      std::string s = "";
      for (auto& iter: list) {
        s += iter.to_string(config, indent + 1);
      }
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