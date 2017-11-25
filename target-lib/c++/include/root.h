#include "object.h"
#include "variable.h"

#include <string>
#include <vector>

#ifndef OGM_ROOT_H
#define OGM_ROOT_H

namespace ogm
{
  class Context;

  class __ObjGlobal: public Object
  {
    const char* get_name() {return "?global"}
  } ObjGlobal;

  /**
    The top-level structure, containing all resource data,
    global variables, etc.
  */
  class Root
  {
  public:
    Root();
    int run(void (*launcher)(Context&));
    
    Instance global;
  };
}

#endif /*OGM_CONTEXT_H*/