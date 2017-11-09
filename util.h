#include <algorithm> 
#include <cctype>
#include <locale>

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

#endif /* UTIL_H */
