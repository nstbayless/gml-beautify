#include "variable.h"

#include <string>
#include <vector>
#include <map>

#ifndef OGM_OBJECT_H
#define OGM_OBJECT_H

namespace ogm
{
  class Object
  {
  public:
    virtual const char* get_name() = 0;
  };

  /**
    An instance of an object
  */
  class Instance
  {
  public:
    Instance(Object& base) : object(&base) { }
    Object* object;
    std::map<VariableID, Variable> local;
  };
}

#endif /*OGM_CONTEXT_H*/