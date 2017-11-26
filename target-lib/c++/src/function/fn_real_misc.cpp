#include "function.h"
#include "variable.h"
#include "error.h"

#include <cassert>
#include <cmath>
#include <algorithm>

using namespace ogm;
using namespace ogm::fn;

VO exp(C, V& v)
{
  return exp(v.get_real());
}

VO ln(C, V& v)
{
  return log(v.get_real());
}

VO power(C, V& base, V& exponent)
{
  return pow(base.get_real(), exponent.get_real());
}

VO sqr(C, V& v)
{
  auto _v = v.get_real();
  return(_v * _v);
}

VO sqrt(C, V& v)
{
  return sqrt(v.get_real());
}

VO log2(C, V& v)
{
  return log2(v.get_real());
}

VO log10(C, V& v)
{
  return log10(v.get_real());
}

VO logn(C, V& v, V& n)
{
  return log2(v.get_real()) / log2(n.get_real());
}

VO int64(C, V& v)
{
  throw NotImplementedError();
  return floor(v.get_real());
}