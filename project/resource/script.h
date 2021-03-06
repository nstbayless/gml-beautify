
#include "resource.h"

#ifndef RESOURCE_SCRIPT_H
#define RESOURCE_SCRIPT_H

class ResScript : public Resource {
public:
  ResScript(std::string path);
  std::string beautify(BeautifulConfig bc, bool dry = false);
  
  std::string path;
};

#endif /*RESOURCE_SCRIPT_H*/
