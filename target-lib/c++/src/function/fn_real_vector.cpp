#include "function.h"
#include "variable.h"

#include <cassert>
#include <algorithm>

using namespace ogm;
using namespace ogm::fn;
/*
VO ogm::fn::point_direction(C, V& x1, V& y1, V& x2, V& y2)
{
  return atan2(x2.get_real() - x1.get_real(), y2.get_real() - y1.get_real());
}

VO ogm::fn::point_distance(C, V& x1, V& y1, V& x2, V& y2)
{
  auto dx = x1.get_real() - x2.get_real();
  auto dy = y1.get_real() - y2.get_real();
  return sqrt(dx * dx
      + dy * dy);
}

VO ogm::fn::point_distance_3d(C, V& x1, V& y1, V& z1, V& x2, V& y2, V& z2)
{
  auto dx = x1.get_real() - x2.get_real();
  auto dy = y1.get_real() - y2.get_real();
  auto dz = z1.get_real() - z2.get_real();
  return sqrt(dx*dx + dy*dy + dz*dz);
}

VO ogm::fn::dot_product(C, V& x1, V& y1, V& x2, V& y2)
{
  return (x1.get_real() * x2.get_real()
    + y1.get_real() * y2.get_real());
}

VO ogm::fn::dot_product_3d(C, V& x1, V& y1, V& z1, V& x2, V& y2, V& z2)
{
  return (x1.get_real() * x2.get_real()
      + y1.get_real() * y2.get_real()
      + z1.get_real() * z2.get_real());
}

VO ogm::fn::dot_product_normalised(C c, V& x1, V& y1, V& x2, V& y2)
{
  // slightly inefficient -- takes sqrt then sqr
  auto dp = dot_product(c, x1, y1, x2, y2);
  return dp / sqr(point_distance(c, x1, y1, x2, y2));
}

VO ogm::fn::dot_product_normalised_3d(C c, V& x1, V& y1, V& z1, V& x2, V& y2, V& z2)
{
  // slightly inefficient -- takes sqrt then sqr
  return dot_product_3d(c, x1, y1, z1, x2, y2, z2) / sqr(point_distance_3d(c, x1, y1, z1, x2, y2, z2));
}
*/
VO ogm::fn::angle_difference(C, V& a1, V& a2)
{
  auto _a1 = a1.get_real();
  auto _a2 = a2.get_real();
  return fmod((fmod((_a2 - _a1) , 360) + 360 + 180), 360) - 180;
}