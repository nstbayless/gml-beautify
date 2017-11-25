#include "function.h"
#include "variable.h"

#include <cassert>
#include <cmath>
#include <algorithm>

using namespace ogm;
using namespace ogm::fn;

const real PI = 3.1415926535897;

VO arccos(C, V& v)
{
  return acos(v.get_real());
}

VO arcsin(C, V& v)
{
  return asin(v.get_real());
}

VO arctan(C, V& v)
{
  return atan(v.get_real());
}

VO arctan2(C, V& x, V& y)
{
  return atan2(x.get_real(), y.get_real());
}

VO sin(C, V& val)
{
  return sin(val.get_real());
}

VO tan(C, V& val)
{
  return tan(val.get_real());
}

VO cos(C, V& val)
{
  return cos(val.get_real());
}

VO darccos(C, V& v)
{
  return radtodeg(acos(v.get_real()));
}

VO darcsin(C, V& v)
{
  return radtodeg(asin(v.get_real()));
}

VO darctan(C, V& v)
{
  return radtodeg(atan(v.get_real()));
}

VO darctan2(C, V& x, V& y)
{
  return radtodeg(atan2(x.get_real(), y.get_real()));
}

VO dsin(C, V& val)
{
  return sin(degtorad(val).get_real());
}

VO dtan(C, V& val)
{
  return tan(degtorad(val).get_real());
}

VO dcos(C, V& val)
{
  return cos(degtorad(val).get_real());
}

VO degtorad(C, V& val)
{
  return val.get_real() * PI / 180.0;
}

VO radtodeg(C, V& val)
{
  return val.get_real() * 180.0 / PI;
}

VO lengthdir_x(C, V& len, V& dir)
{
  return cos(dir.get_real()) * len.get_real();
}

VO lengthdir_y(C, V& len, V& dir)
{
  return -1.0 * sin(dir.get_real()) * len.get_real();
}

VO pi(C)
{
  return PI;
}