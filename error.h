#include <algorithm> 
#include <cctype>
#include <locale>
#include <sstream>
#include <fstream>
#include <string>

#ifndef ERROR_H
#define ERROR_H

//! when parsing causes an error
class ParseError : public std::exception {
public:
  ParseError(std::string message, std::pair<int,int> location) : message(message), location(location) { }
  virtual const char* what() const noexcept override {
    return (message + "\n  at row "
            + std::to_string(location.first)
            + ", column " + std::to_string(location.second)).c_str();
  }
private:
  std::string message;
  std::pair<int,int> location;
};

//! when testing causes an error
class TestError : public std::exception {
public:
  TestError(std::string message) : message(message) { }
  virtual const char* what() const noexcept override {
    return message.c_str();
  }
private:
  std::string message;
};

//! when I'm too lazy to fi
class MiscError : public std::exception {
public:
  MiscError(std::string message) : message(message) { }
  virtual const char* what() const noexcept override {
    return message.c_str();
  }
private:
  std::string message;
};

#endif /*ERROR_H*/