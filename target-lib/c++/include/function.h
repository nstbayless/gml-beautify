#include "variable.h"
#include "context.h"

#ifndef OGMFN_H
#define OGMFN_H

#define DEF(...) constexpr VariableID __VA_ARGS__ = -1 * __LINE__;

namespace ogm
{
  typedef Context& C;
  typedef const Variable V;
  typedef Variable VO;
  namespace fn
  {
    #include "fn/fn_math.h"
    #include "fn/fn_string.h"
    #include "fn/fn_debug.h"
  }
  namespace prop
  {
    #include "fn/prop.h"
  }
  namespace varn
  {
    #include "fn/var.h"
  }
}

#undef DEF

#endif /*OGMFN_H*/