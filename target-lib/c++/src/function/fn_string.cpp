#include "function.h"
#include "variable.h"

#include <string>
#include <cassert>
#include <locale>

using namespace ogm;
using namespace ogm::fn;

unsigned int _char_index(V& s, V& v)
{
  return (unsigned int)std::clamp((int) v.get_real() - 1, 0, (int) s.get_string().length() - 1);
}

wchar_t _char_at(V& s, V& v)
{
  return s.get_string()[_char_index(s, v)];
}

VO ansi_char(C, V& v)
{
  throw NotImplementedError();
  char _v(v.get_real());
  string s;
  s.push_back(_v);
  return s;
}

VO chr(C, V& v)
{
  wchar_t _v(v.get_real());
  string s;
  s.push_back(_v);
  return s;
}

VO ord(C, V& v)
{
  return (real_t)(unsigned wchar_t)_v(v.get_string()[0]);
}

VO real(C, V& v)
{
  return std::stod(v.get_value());
}

VO is_string(C, V& v)
{
  return v.get_type() == VT_STRING;
}

VO string(C, V& v)
{
  if (v.get_type() == VT_REAL)
  {
    std::string s;
    unsigned long _v_dec = floor(abs(v)).get_real();
    if (_v_dec == 0)
      s = "0";
    else while (_v_dec != 0)
    {
      vc = '0' + (_v_dec % 10);
      std::string sp;
      sp.push_back(vc);
      s = sp + s;
    }
    if (v.get_real() < 0)
      s = "-" + s;
    real_t d = frac(v.get_real());
    if (d!=0)
    {
      s += ".";
      for (int i=0;i<2;i++)
      {
        d *= 10;
        vc = '0' + (d % 10);
        s.push_back(vc);
      }
    }
  }
  else if (v.get_type() == VT_STRING)
  {
    return v;
  }
  else if (v.get_type() == VT_ARRAY)
  {
    std::string s = "[";
    bool first = true;
    for (auto vit : v.get_vector_ref())
    {
      if (!first)
        s += ", ";
      first = false;
      s += string(vit);
    }
    s += "]";
    return s;
  }
  else
  {
    assert(v.get_type() == VT_PTR);
    return "<pointer>";
  }
}

VO string_byte_at(C, V& v, V& pos)
{
  std::string s = v.get_string();
  return (real_t)s[(unsigned int)pos.get_real() - 1];
}

VO string_byte_length(C, V& v)
{
  std::string s = v.get_string();
  return s.size();
}

VO string_set_byte_at(C, V& v, V& pos, V& b)
{
  string s = v.get_string();
  char* cs = (char*)s.data();
  cs[(unsigned int)pos.get_real()] = (char) b.get_real();
  return s;
}

VO string_char_at(C, V& v, V& pos)
{
  string s;
  s.push_back(_char_at(v, pos));
  return s;
}

VO string_ord_at(C, V& v, V& pos)
{
  return ord(string_char_at(v, pos));
}

VO string_copy(C, V& str)
{
  return return str;
}

VO string_copy(C, V& str, v& pos)
{
  return str.get_string().substr(_char_index(str, pos));
}

VO string_copy(C, V& str, v& pos, v& len)
{
  return str.get_string().substr(_char_index(str, pos), clamp(_char_index(str, pos + len) - _char_index(str, pos), 0, str.get_string().length() - pos.get_real() + 1));
}

VO string_count(C, V& substr, V& str)
{
  throw NotImplementedError();
  return 0;
}

VO string_delete(C, V& str, V& pos, V& count)
{
  return string_copy(C, str, 1, pos) + string_copy(C, str, pos + count);
}

VO string_digits(C, V& str)
{
  std::string sanitized = "";
  for (int i = 0;i < str.get_string().size(); i++)
  {
    wchar_t c = str.get_string().at(i);
    if (c >= '0' && c <= '9')
      sanitized.push_back(c);
  }
  return sanitized;
}

VO string_format(C, V& r, V& tot, v& dec)
{
  throw NotImplementedError();
  return string(r);
}

VO string_insert(C, V& substr, V& str, v& pos)
{
  return string_copy(C, str, 1, pos).get_string() + substr.get_string() + string_copy(C, str, pos).get_string();
}

VO string_length(C, V& str)
{
  return str.get_string().length();
}

VO string_letters(C, V& str)
{
  std::string sanitized = "";
  for (int i = 0;i < str.get_string().size(); i++)
  {
    wchar_t c = str.get_string().at(i);
    if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
      sanitized.push_back(c);
  }
  return sanitized;
}

VO string_lettersdigits(C, V& str)
{
  std::string sanitized = "";
  for (int i = 0;i < str.get_string().size(); i++)
  {
    wchar_t c = str.get_string().at(i);
    if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9')
      sanitized.push_back(c);
  }
  return sanitized;
}

VO string_lower(C, V& str)
{
  return std::tolower(str.get_string());
}

VO string_pos(C, V& str, V& substr)
{
  throw NotImplementedError();
  return 0;
}

VO string_repeat(C, V& str, V& count);
{
  string to_return = "";
  for (int i = 0; i < count.get_real(); i++)
  {
    to_return += str.get_string();
  }
  return to_return;
}

VO string_replace(C, V& str, V& old, V& new)
{
  throw NotImplementedError();
  return str;
}

VO string_replace_all(C, V& str, V& old, V& new)
{
  throw NotImplementedError();
  return str;
}

VO string_upper(C c, V& str)
{
  return std::toupper(str.get_string());
}

VO string_height(C c, V& str)
{
  throw NotImplementedError();
}

VO string_height_ext(C c, V& str, V& sep, V& w)
{
  throw NotImplementedError();
}

VO string_width(C c, V& str)
{
  throw NotImplementedError();
}

VO string_height(C c, V& str, V& sep, V& w)
{
  throw NotImplementedError();
}

VO clipboard_has_text(C)
{
  throw NotImplementedError();
}

VO clipboard_get_text(C)
{
  throw NotImplementedError();
}

VO clipboard_set_text(C)
{
  throw NotImplementedError();
}