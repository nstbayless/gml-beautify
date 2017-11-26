#include "function.h"
#include "variable.h"

#include <cassert>
#include <algorithm>

using namespace ogm;
using namespace ogm::fn;

VO round(C c, V& v)
{
  return round(v.get_real());
}

VO floor(C c, V& v)
{
  return floor(v.get_real());
}

VO frac(C c, V& v)
{
  return fmod(v.get_real(),1.0);
}

VO abs(C c, V& v)
{
  return abs(v.get_real());
}

VO sign(C c, V& v)
{
  auto _v =  v.get_real();
  if (_v == 0)
    return 0;
  if (_v < 0)
    return -1;
  return 1;
}

VO ceil(C c, V& v)
{
  return std::ceil(v.get_real());
}

VO max(C c, byte n, V* v)
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

VO mean(C, byte n, V* v)
{
  assert(n >= 1);
  auto sum = 0;
  for (byte i=0; i < n; i++)
  {
    sum += v[i].get_real();
  }
  return sum/(double)n;
}

VO median(C, byte n, V* v)
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

VO min(C, byte n, V* v)
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

VO clamp(C c, V& val, V& min, V& max)
{
  if (val < min)
    return min;
  if (val >= max)
    return max;
  return val;
}

VO lerp(C c, V& a, V& b, V& amt)
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