#include "lbstring.h"
#include "production.h"

#ifndef COMPILE_H
#define COMPILE_H

struct CompilerContext {
  std::string runtime_context = "c";
  int varl = 0;
};

LBString compileModule(const PrBody& p);

#endif /*COMPILE_H*/
