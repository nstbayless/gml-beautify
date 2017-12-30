#include "compile.h"
#include "production.h"
#include "util.h"
#include "error.h"

#include <iostream>

#define DEF(...) builtin.insert(ext_trim(#__VA_ARGS__));

CompilerGlobalContext::CompilerGlobalContext()
{
  // add Built-In variables to the set
  #include "target-lib/c++/include/fn/var.h"
  for (auto vn : builtin)
    instvars.insert(vn);
}

#undef DEF

CompilerContext::CompilerContext(CompilerGlobalContext* global): global(global)
{ }