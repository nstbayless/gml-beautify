#include "variable.h"

#ifndef OGMFN_H
#define OGMFN_H

namespace ogm {
  typedef void* C;
  typedef const Variable V;
  typedef Variable VO;
  namespace fn {
    #include "functionlist.h"
  }
}

#endif /*OGMFN_H*/
