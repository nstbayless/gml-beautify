#include "variable.h"
#include "context.h"

#ifndef OGMFN_H
#define OGMFN_H

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
}

#endif /*OGMFN_H*/