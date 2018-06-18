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

// trim from both ends (not in place)
static inline std::string ext_trim(std::string s)
{
  trim(s);
  return s;
}

// trim from end (not in place)
static inline std::string ext_rtrim(std::string s)
{
  rtrim(s);
  return s;
}

static std::string path_leaf(std::string path) {
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

static std::string path_directory(std::string path) {
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

// from https://stackoverflow.com/a/24315631
static std::string replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

// converts / and \\ to native path separator
static inline std::string native_path(std::string path) {
  #ifdef _WIN32
  return replace_all(path,"/","\\");
  #elif defined __unix__
  return replace_all(path,"\\","/");
  #endif
}

static inline std::string read_file_contents(std::string path_to_file) {
  std::string line;
  std::string out;
  
  std::ifstream infile(native_path(path_to_file));
  while (getline(infile, line))
  {
      out += line + "\n";
  }
  
  return out;
}

static inline std::string read_file_contents(std::ifstream& infile) {
  std::string out;
  std::string line;
  while (getline(infile, line))
  {
      out += line + "\n";
  }
  
  return out;
}

static std::pair<int,int> first_difference(std::string a, std::string b) {
  if (a == b)
    return std::pair<int, int>(-1,-1);
  
  int line = 1;
  for (int x=0;x<std::min(a.size(),b.size());x++) {
    if (a[x] != b[x])
      return std::pair<int, int>(x,line);
    if (a[x] == '\n')
      line ++;
  }
  
  return std::pair<int, int> (
    std::min(a.size(),b.size()),
    line
  );
}

static bool ends_with(const std::string& full, const std::string& suffix) {
  if (suffix.length() > full.length())
    return false;
  return (full.substr(full.length() - suffix.length(), suffix.length()) == suffix);
}

static bool starts_with(const std::string& full, const std::string& prefix) {
  if (prefix.length() > full.length())
    return false;
  return (full.substr(0, prefix.length()) == prefix);
}

#endif /* UTIL_H */
