#include "function.h"
#include "variable.h"
#include "error.h"

#include <string>
#include <cassert>
#include <locale>
#include <cctype>

using namespace ogm;
using namespace ogm::fn;

unsigned int _char_index(V& s, V& v)
{
  if (v.get_real() < 1)
    return 0;
  if (v.get_real() >= s.get_string().length())
    return s.get_string().length() - 1;
  return (unsigned int)(v.get_real() - 1);
}

char_t _char_at(V& s, V& v)
{
  return s.get_string()[_char_index(s, v)];
}

VO ogm::fn::ansi_char(C, V& v)
{
  throw NotImplementedError();
  char _v(v.get_real());
  string_t s;
  s.push_back(_v);
  return s;
}

VO ogm::fn::chr(C, V& v)
{
  char_t _v(v.get_real());
  string_t s;
  s.push_back(_v);
  return s;
}

VO ogm::fn::ord(C, V& v)
{
  return (real_t)(unsigned char_t)(v.get_string()[0]);
}

VO ogm::fn::real(C, V& v)
{
  return std::stod(v.get_string());
}

VO ogm::fn::is_string(C, V& v)
{
  return v.get_type() == VT_STRING;
}

VO ogm::fn::string(C c, V& v)
{
  if (v.get_type() == VT_REAL)
  {
    std::string s;
    unsigned long _v_dec = floor(c, abs(c, v)).get_real();
    if (_v_dec == 0)
      s = "0";
    else while (_v_dec != 0)
    {
      char vc = '0' + (_v_dec % 10);
      std::string sp;
      sp.push_back(vc);
      s = sp + s;
      _v_dec /= 10l;
    }
    if (v.get_real() < 0)
      s = "-" + s;
    real_t d = frac(c, v).get_real();
    if (d!=0)
    {
      s += ".";
      for (int i=0;i<2;i++)
      {
        d *= 10;
        char vc = '0' + (((int)floor(c, d).get_real()) % 10);
        s.push_back(vc);
      }
    }
    return s;
  }
  else if (v.get_type() == VT_STRING)
  {
    return v;
  }
  else if (v.get_type() == VT_ARRAY)
  {
    string_t s;
    s.push_back('[');
    bool first = true;
    for (auto vit : v.get_vector_ref())
    {
      if (!first) {
        s.push_back(',');
        s.push_back(' ');
      }
      first = false;
      s += ogm::fn::string(c, vit).get_string();
    }
    s.push_back(']');
    return s;
  }
  else
  {
    assert(v.get_type() == VT_PTR);
    return "<pointer>";
  }
}

VO ogm::fn::string_byte_at(C, V& v, V& pos)
{
  std::string s = (char*)v.get_string().data();
  return (real_t)s[(unsigned int)pos.get_real() - 1];
}

VO ogm::fn::string_byte_length(C, V& v)
{
  std::string s = (char*)v.get_string().data();
  return (real_t)s.length();
}

VO ogm::fn::string_set_byte_at(C, V& v, V& pos, V& b)
{
  std::string s = (char*)v.get_string().data();
  char* cs = (char*)s.data();
  cs[(unsigned int)pos.get_real()] = (char) b.get_real();
  return (char_t*)s.c_str();
}

VO ogm::fn::string_char_at(C, V& v, V& pos)
{
  string_t s;
  s.push_back(_char_at(v, pos));
  return s;
}

VO ogm::fn::string_ord_at(C c, V& v, V& pos)
{
  var ch = ogm::fn::string_char_at(c, v, pos);
  return ogm::fn::ord(c, ch);
}

VO ogm::fn::string_copy(C, V& str)
{
  return str;
}

VO ogm::fn::string_copy(C, V& str, V& pos)
{
  return str.get_string().substr(_char_index(str, pos));
}

VO ogm::fn::string_copy(C c, V& str, V& pos, V& len)
{
  if (len.get_real() + pos.get_real() >= str.get_string().length())
    return ogm::fn::string_copy(c, str, pos);
  if (len <= var(0))
    return (char_t*)"";
  return str.get_string().substr(_char_index(str, pos), (int)len.get_real());
}

VO ogm::fn::string_count(C, V& substr, V& str)
{
  throw NotImplementedError();
  return 0;
}

VO ogm::fn::string_delete(C c, V& str, V& pos, V& count)
{
  return ogm::fn::string_copy(c, str, 1, pos) + ogm::fn::string_copy(c, str, pos + count);
}

VO ogm::fn::string_digits(C, V& str)
{
  string_t sanitized;
  for (int i = 0;i < str.get_string().size(); i++)
  {
    char_t c = str.get_string().at(i);
    if (c >= '0' && c <= '9')
      sanitized.push_back(c);
  }
  return sanitized;
}

VO ogm::fn::string_format(C c, V& r, V& tot, V& dec)
{
  throw NotImplementedError();
  return ogm::fn::string(c, r);
}

VO ogm::fn::string_insert(C c, V& substr, V& str, V& pos)
{
  return ogm::fn::string_copy(c, str, 1, pos).get_string() + substr.get_string() + ogm::fn::string_copy(c, str, pos).get_string();
}

VO ogm::fn::string_length(C, V& str)
{
  return (int)str.get_string().length();
}

VO ogm::fn::string_letters(C, V& str)
{
  string_t sanitized;
  for (int i = 0;i < str.get_string().size(); i++)
  {
    char_t c = str.get_string().at(i);
    if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
      sanitized.push_back(c);
  }
  return sanitized;
}

VO ogm::fn::string_lettersdigits(C, V& str)
{
  string_t sanitized;
  for (int i = 0;i < str.get_string().size(); i++)
  {
    char_t c = str.get_string().at(i);
    if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9')
      sanitized.push_back(c);
  }
  return sanitized;
}

VO ogm::fn::string_lower(C, V& str)
{
  throw NotImplementedError();
  return str;
}

VO ogm::fn::string_pos(C, V& str, V& substr)
{
  throw NotImplementedError();
  return 0;
}

VO ogm::fn::string_repeat(C, V& str, V& count)
{
  string_t to_return;
  for (int i = 0; i < count.get_real(); i++)
  {
    to_return += str.get_string();
  }
  return to_return;
}

VO ogm::fn::string_replace(C, V& str, V& old, V& _new)
{
  throw NotImplementedError();
  return str;
}

VO ogm::fn::string_replace_all(C, V& str, V& old, V& _new)
{
  throw NotImplementedError();
  return str;
}

VO ogm::fn::string_upper(C c, V& str)
{
  throw NotImplementedError();
  return str;
}

VO ogm::fn::string_height(C c, V& str)
{
  throw NotImplementedError();
}

VO ogm::fn::string_height_ext(C c, V& str, V& sep, V& w)
{
  throw NotImplementedError();
}

VO ogm::fn::string_width(C c, V& str)
{
  throw NotImplementedError();
}

VO ogm::fn::string_height(C c, V& str, V& sep, V& w)
{
  throw NotImplementedError();
}

VO ogm::fn::clipboard_has_text(C)
{
  throw NotImplementedError();
}

VO ogm::fn::clipboard_get_text(C)
{
  throw NotImplementedError();
}

VO ogm::fn::clipboard_set_text(C)
{
  throw NotImplementedError();
}