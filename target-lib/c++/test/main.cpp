#include "root.h"
#include "variable.h"
#include "function.h"

using namespace ogm;

void launcher(C c)
{
  var x = 4.549;
  ogm::fn::show_debug_message(c, "Hello, world!");
  ogm::fn::show_debug_message(c, x);
}

int main (int argn, char** argv) {
  Root root;
  root.run(launcher);
}