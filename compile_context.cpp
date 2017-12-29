#include "compile.h"
#include "production.h"
#include "util.h"
#include "error.h"

#define DEF(...) builtin.insert(ext_trim(#__VA_ARGS__));

CompilerGlobalContext::CompilerGlobalContext()
{
  // add Built-In variables to the set
  #include "target-lib/c++/include/fn/var.h"
  for (auto vn : builtin)
    instvars.insert(vn);
}

#undef DEF

CompilerContext::CompilerContext()
{
  global = new CompilerGlobalContext();
}

CompilerContext::~CompilerContext()
{
  delete(global);
}