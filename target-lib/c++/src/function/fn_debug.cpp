#include "function.h"
#include "variable.h"
#include "error.h"

#include <string>
#include <cassert>
#include <locale>

using namespace ogm;
using namespace ogm::fn;

VO debug_mode(C)
{
  throw NotImplementedError();
  return false;
}

VO get_integer(C c, V& prompt)
{
  return get_integer(c, prompt, 0);
}

VO get_integer(C c, V& prompt, V& def)
{
  return round(real_t(get_string(c, prompt, string(def))));
}

VO get_string(C c, V& prompt)
{
  return get_string(c, prompt, "");
}

VO get_string(C, V& prompt, V& def)
{
  std::cout<<prompt.get_string()<<"> ";
  string s;
  std::cin>>s;
  return s;
}

VO show_message(C, V& msg)
{
  std::cout<<msg.get_string()<<std::endl;
}

VO show_question(C, V& msg)
{
  while (true)
  {
    string in(show_message(C, msg + string(" [y/n]")).get_string());
    if (in == "y" || in == "Y")
      return true;
    if (in == "n" || in == "N")
      return false;
  }
}

void show_debug_message(C, V& msg)
{
  std::cout<<string(msg).get_string()<<std::endl;
}

void show_debug_overlay(C, V& enable)
{
  throw NotImplementedError();
}

VO code_is_compiled(C)
{
  return true;
}

VO fps(C)
{
  throw NotImplementedError();
  return 60;
}

VO fps_real(C)
{
  throw NotImplementedError();
  return 60;
}
