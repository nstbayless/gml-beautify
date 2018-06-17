#include "function.h"
#include "variable.h"

#include <cassert>
#include <cmath>
#include <algorithm>

using namespace ogm;
using namespace ogm::fn;

const real_t PI = 3.1415926535897;

VO ogm::fn::arccos(C, V& v)
{
  return acos(v.get_real());
}

VO ogm::fn::arcsin(C, V& v)
{
  return asin(v.get_real());
}

VO ogm::fn::arctan(C, V& v)
{
  return atan(v.get_real());
}

VO ogm::fn::arctan2(C, V& x, V& y)
{
  return atan2(x.get_real(), y.get_real());
}

VO ogm::fn::sin(C, V& val)
{
  return std::sin(val.get_real());
}

VO ogm::fn::tan(C, V& val)
{
  return std::tan(val.get_real());
}

VO ogm::fn::cos(C, V& val)
{
  return std::cos(val.get_real());
}

VO ogm::fn::darccos(C c, V& v)
{
  return radtodeg(c, acos(v.get_real()));
}

VO ogm::fn::darcsin(C c, V& v)
{
  return radtodeg(c, asin(v.get_real()));
}

VO ogm::fn::darctan(C c, V& v)
{
  return radtodeg(c, atan(v.get_real()));
}

VO ogm::fn::darctan2(C c, V& x, V& y)
{
  return radtodeg(c, atan2(x.get_real(), y.get_real()));
}

VO ogm::fn::dsin(C c, V& val)
{
  return std::sin(degtorad(c, val).get_real());
}

VO ogm::fn::dtan(C c, V& val)
{
  return std::tan(degtorad(c, val).get_real());
}

VO ogm::fn::dcos(C c, V& val)
{
  return std::cos(degtorad(c, val).get_real());
}

VO ogm::fn::degtorad(C, V& val)
{
  return val.get_real() * PI / 180.0;
}

VO ogm::fn::radtodeg(C, V& val)
{
  return val.get_real() * 180.0 / PI;
}

VO ogm::fn::lengthdir_x(C, V& len, V& dir)
{
  return std::cos(dir.get_real()) * len.get_real();
}

VO ogm::fn::lengthdir_y(C, V& len, V& dir)
{
  return -1.0 * std::sin(dir.get_real()) * len.get_real();
}

VO ogm::prop::pi(C)
{
  return PI;
}