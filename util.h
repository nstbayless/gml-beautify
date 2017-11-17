#include <algorithm> 
#include <cctype>
#include <locale>
#include <sstream>
#include <fstream>
#include <string>

#ifndef UTIL_H
#define UTIL_H

template<class Base, class Any>
bool is_a(Any* ptr) {
  return !! dynamic_cast<Base*>(ptr);
}


// trim from https://stackoverflow.com/a/217605

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

class ParseError : public std::runtime_error {
public:
  ParseError(std::string message);
  virtual const char* what() const noexcept override {
    return message.c_str();
  }
private:
  std::string message;
};

static std::string path_leaf(std:: string path) {
  size_t last_bsl = path.find_last_of("\\");
  size_t last_rsl = path.find_last_of("/");
  
  size_t sep;
  
  if (last_bsl == std::string::npos) {
    if (last_rsl == std::string::npos)
      return "";
    sep = last_rsl;
  }
  else if (last_rsl == std::string::npos) {
    sep = last_bsl;
  } else {
    sep = std::max(last_rsl, last_bsl);
  }
  
  return path.substr(sep+1,path.length() - sep-1);
}

static std::string path_directory(std:: string path) {
  size_t last_bsl = path.find_last_of("\\");
  size_t last_rsl = path.find_last_of("/");
  
  size_t sep;
  
  if (last_bsl == std::string::npos) {
    if (last_rsl == std::string::npos)
      return "";
    sep = last_rsl;
  }
  else if (last_rsl == std::string::npos) {
    sep = last_bsl;
  } else {
    sep = std::max(last_rsl, last_bsl);
  }
  
  return path.substr(0,sep+1);
}

static inline std::string read_file_contents(std::string path_to_file) {
  std::string line;
  std::string out;
  
  std::ifstream infile(path_to_file);
  while (getline(infile, line))
  {
      out += line+"\n";
  }
  
  return out;
}

static inline std::string read_file_contents(std::ifstream& infile) {
  std::string out;
  std::string line;
  while (getline(infile, line))
  {
      out += line+"\n";
  }
  
  return out;
}

#endif /* UTIL_H */
