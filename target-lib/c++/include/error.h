#include <algorithm> 
#include <cctype>
#include <locale>
#include <string>

#ifndef OGM_ERROR_H
#define OGM_ERROR_H

class TypeError : public std::exception {
public:
  TypeError(std::string message) : message(message) { }
  virtual const char* what() const noexcept override {
    return message.c_str();
  }
private:
  std::string message;
}

class NotImplementedError : public std::exception {
public:
  TypeError() : message("function not yet implemented in OGM") { }
  virtual const char* what() const noexcept override {
    return message.c_str();
  }
private:
  std::string message;
}

#endif /*OGM_ERROR_H*/