#include "function.h"
#include "variable.h"

#include <cassert>
#include <algorithm>

using namespace ogm;
using namespace ogm::fn;

VO ogm::fn::round(C c, V& v)
{
  return floor(c, v.get_real() + .5);
}

VO ogm::fn::floor(C c, V& v)
{
  return std::floor(v.get_real());
}

VO ogm::fn::frac(C c, V& v)
{
  return fmod(v.get_real(),1.0);
}

VO ogm::fn::abs(C c, V& v)
{
  return std::abs(v.get_real());
}

VO ogm::fn::sign(C c, V& v)
{
  auto _v =  v.get_real();
  if (_v == 0)
    return 0;
  if (_v < 0)
    return -1;
  return 1;
}

VO ogm::fn::ceil(C c, V& v)
{
  return std::ceil(v.get_real());
}

VO ogm::fn::max(C c, byte n, V* v)
{
  assert(n >= 1);
  auto to_return = v[0].get_real();
  for (byte i=1; i<n; i++)
  {
    if (v[i].get_real() >= to_return)
      to_return = v[i].get_real();
  }
  return to_return;
}

VO ogm::fn::mean(C, byte n, V* v)
{
  assert(n >= 1);
  auto sum = 0;
  for (byte i=0; i < n; i++)
  {
    sum += v[i].get_real();
  }
  return sum/(double)n;
}

VO ogm::fn::median(C, byte n, V* v)
{
  assert(n >= 1);
  byte req = n/2;
  for (byte i=0;i<n;i++)
  {
    auto cmp = v[i].get_real();
    byte found = 0;
    for (byte j=0;j<n;j++)
    {
      if (cmp < v[j].get_real())
        found ++;
    }
    if (found == req)
      return cmp;
  }
  assert(false);
}

VO ogm::fn::min(C, byte n, V* v)
{
  assert(n >= 1);
  auto to_return = v[0].get_real();
  for (byte i=1; i<n; i++)
  {
    if (v[i].get_real() < to_return)
      to_return = v[i].get_real();
  }
  return to_return;
}

VO ogm::fn::clamp(C c, V& val, V& min, V& max)
{
  if (val < min)
    return min;
  if (val >= max)
    return max;
  return val;
}

VO ogm::fn::lerp(C c, V& a, V& b, V& amt)
{
  if (amt < 0)
    return a;
  if (amt >= b)
    return b;
   
  auto _a = a.get_real();
  auto _b = b.get_real();   
  auto _amt = amt.get_real();
  return _amt * (_b - _a) + _a;
}