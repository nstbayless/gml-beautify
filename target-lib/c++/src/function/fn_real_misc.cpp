#include "function.h"
#include "variable.h"
#include "error.h"

#include <cassert>
#include <cmath>
#include <algorithm>

using namespace ogm;
using namespace ogm::fn;

VO ogm::fn::exp(C, V& v)
{
  return std::exp(v.get_real());
}

VO ogm::fn::ln(C, V& v)
{
  return std::log(v.get_real());
}

VO ogm::fn::power(C, V& base, V& exponent)
{
  return std::pow(base.get_real(), exponent.get_real());
}

VO ogm::fn::sqr(C, V& v)
{
  real_t _v = v.get_real();
  return(_v * _v);
}

VO ogm::fn::sqrt(C, V& v)
{
  return std::sqrt(v.get_real());
}

VO ogm::fn::log2(C, V& v)
{
  return std::log2(v.get_real());
}

VO ogm::fn::log10(C, V& v)
{
  return std::log10(v.get_real());
}

VO ogm::fn::logn(C, V& v, V& n)
{
  return std::log2(v.get_real()) / std::log2(n.get_real());
}
