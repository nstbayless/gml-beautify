#include "function.h"
#include "variable.h"

#include <cassert>
#include <algorithm>

using namespace ogm;
using namespace ogm::fn;

VO point_direction(C, V& x1, V& y1, V& x2, V& y2)
{
  return atan2(x2.get_real() - x1.get_real(), y2.get_real() - y1.get_real());
}

VO point_distance(C, V& x1, V& y1, V& x2, V& y2)
{
  return sqrt(sqr(x1.get_real() - x2.get_real())
      + sqr(y1.get_real() - y2.get_real()));
}

VO point_distance_3d(C, V& x1, V& y1, V& z1, V& x2, V& y2, V& z2)
{
  return sqrt(sqr(x1.get_real() - x2.get_real())
      + sqr(y1.get_real() - y2.get_real())
      + sqr(z1.get_real() - z2.get_real()));
}

VO dot_product(C, V& x1, V& y1, V& x2, V& y2)
{
  return (x1.get_real() * x2.get_real()
    + y1.get_real() * y2.get_real())
}

VO dot_product_3d(C, V& x1, V& y1, V& z1, V& x2, V& y2, V& z2)
{
  return (x1.get_real() * x2.get_real()
      + y1.get_real() * y2.get_real()
      + z1.get_real() * z2.get_real())
}

VO dot_product_normalised(C, V& x1, V& y1, V& x2, V& y2)
{
  // slightly inefficient -- takes sqrt then sqr
  return dot_product(C, x1, y1, x2, y2) / sqr(point_distance(C, x1, y1, x2, y2));
}

VO dot_product_normalised_3d(C, V& x1, V& y1, V& z1, V& x2, V& y2, V& z2)
{
  // slightly inefficient -- takes sqrt then sqr
  return dot_product_3d(C, x1, y1, z1, x2, y2, z2) / sqr(point_distance_3d(C, x1, y1, z1, x2, y2, z2));
}

VO angle_difference(C, V& a1, V& a2)
{
  real _a1 = a1.get_real();
  real _a2 = a2.get_real();
  return ((((_a2 - _a1) % 360) + 360 + 180) % 360) - 180;
}