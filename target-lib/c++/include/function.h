#include "variable.h"
#include "context.h"

#ifndef OGMFN_H
#define OGMFN_H

#define BIVAR = -1 * (__LINE__ - 1),
#define BIVAR_END = -1 * (__LINE__ - 1);

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
    constexpr VariableID
    #include "fn/var.h"
  }
}

#endif /*OGMFN_H*/