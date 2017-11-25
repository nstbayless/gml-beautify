#include "function.h"
#include "variable.h"

#include <cassert>
#include <algorithm>

using namespace ogm;
using namespace ogm::fn;

VO round(C c, V& v)
{
  return std::round(v.get_real());
}

VO floor(C c, V& v)
{
  return std::floor(v.get_real());
}

VO frac(C c, V& v)
{
  return fmod(v.get_real(),1.0);
}

VO abs(C c, V& v)
{
  return std::abs(v.get_real());
}

VO sign(C c, V& v)
{
  return std::sign(v.get_real());
}

VO ceil(C c, V& v)
{
  return std::ceil(v.get_real());
}

VO max(C c, byte n, V&* v)
{
  assert(n >= 1);
  real to_return = v[0].get_real();
  for (byte i=1; i<n; i++)
  {
    if (v[j].get_real() >= to_return)
      to_return = v[j].get_real();
  }
  return to_return;
}

VO mean(C, byte n, V&* v)
{
  assert(n >= 1);
  real sum = 0;
  for (byte i=0; i < n; i++)
  {
    sum += v[i];
  }
  return sum/(double)n;
}

VO median(C, byte n, V&*)
{
  assert(n >= 1);
  byte req = n/2;
  for (byte i=0;i<n;i++)
  {
    real cmp = v[i].get_real();
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

VO min(C, byte n, V&*)
{
  assert(n >= 1);
  real to_return = v[0].get_real();
  for (byte i=1; i<n; i++)
  {
    if (v[j].get_real() < to_return)
      to_return = v[j].get_real();
  }
  return to_return;
}

VO clamp(C c, V& val, V& min, V& max)
{
  return std::clamp(val.get_real(), min.get_real(), max.get_real());
}

VO lerp(C c, V& a, V& b, V& amt)
{
  real _a = a.get_real();
  real _b = b.get_real();
  real _amt = std::clamp(amt.get_real(), 0, 1);
  return _amt * (_b - _a) + _a;
}