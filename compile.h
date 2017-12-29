#include "lbstring.h"
#include "production.h"

#include <map>
#include <set>

#ifndef COMPILE_H
#define COMPILE_H

class CompilerGlobalContext {
public:
  int varnames_n = 0;
  std::set<std::string> builtin;
  std::set<std::string> instvars;
  
  CompilerGlobalContext();
};

class CompilerContext {
public:
  std::string runtime_context = "c";
  std::set<std::string> localvars;
  int varl = 0;
  
  CompilerGlobalContext* global;
  
  CompilerContext();
  ~CompilerContext();
};

LBString compileModule(const PrBody& p);

#endif /*COMPILE_H*/
