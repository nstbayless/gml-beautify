#include "object.h"
#include "root.h"

#include <string>
#include <vector>

#ifndef OGM_CONTEXT_H
#define OGM_CONTEXT_H

namespace ogm
{
  /**
    The instance, room, and root.
  */
  class Context
  {
    Instance* instance = nullptr;
    Root* root = nullptr;
  };
}

#endif /*OGM_CONTEXT_H*/