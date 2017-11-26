#include "function.h"
#include "variable.h"
#include "error.h"

#include <string>
#include <cassert>
#include <locale>
#include <iostream>

using namespace ogm;
using namespace ogm::fn;

VO ogm::fn::debug_mode(C)
{
  throw NotImplementedError();
  return false;
}

VO ogm::fn::get_integer(C c, V& prompt)
{
  return get_integer(c, prompt, 0);
}

VO ogm::fn::get_integer(C c, V& prompt, V& def)
{
  var str = string(c, def);
  var in = get_string(c, prompt, str);
  return round(c, real(c, in));
}

VO ogm::fn::get_string(C c, V& prompt)
{
  return get_string(c, prompt, "");
}

VO ogm::fn::get_string(C, V& prompt, V& def)
{
  std::wcout<<prompt.get_string()<<"> ";
  string_t s;
  std::wcin>>s;
  return s;
}

void ogm::fn::show_message(C, V& msg)
{
  std::wcout<<msg.get_string()<<std::endl;
}

VO ogm::fn::show_question(C c, V& msg)
{
  throw NotImplementedError();
  /*while (true)
  {
    string_t prompt = msg.get_string();
    prompt += (wchar_t*) " [y/n]";
    var prompt_v(prompt);
    string_t in(get_string(c, prompt_v).get_string());
    if (in == "y" || in == "Y")
      return true;
    if (in == "n" || in == "N")
      return false;
  }*/
  return 0;
}

void ogm::fn::show_debug_message(C c, V& msg)
{
  std::wcout<<string(c, msg).get_string()<<std::endl;
}

void ogm::fn::show_debug_overlay(C, V& enable)
{
  throw NotImplementedError();
}

VO ogm::fn::code_is_compiled(C)
{
  return true;
}

VO ogm::fn::fps(C)
{
  throw NotImplementedError();
  return 60;
}

VO ogm::fn::fps_real(C)
{
  throw NotImplementedError();
  return 60;
}
