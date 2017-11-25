#include "root.h"

using namespace ogm;

class __ObjLaunchObject: public Object
{
  const char* get_name() {return "?instance"}
} ObjLaunchObject;

Root::Root(): global(ObjGlobal)
{
  
}

int Root::run(void (*launch)(Context&))
{
  Instance launchInstance(ObjLaunchObject);
  Context c;
  c.instance = &launchInstance;
  c.root = this;
  
  launch(c);
  
  return true;
}