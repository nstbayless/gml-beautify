
#include "resource.h"

#ifndef RESOURCE_OBJECT_H
#define RESOURCE_OBJECT_H

class ResObject : public Resource {
public:
  ResObject(std::string path);
  std::string path;
};

#endif /*RESOURCE_OBJECT_H*/
