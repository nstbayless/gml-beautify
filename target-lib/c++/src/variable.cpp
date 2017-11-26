#include "variable.h"
#include "error.h"

using namespace ogm;

ogm::Variable::Variable(real_t r)
{
  set(r);
}

ogm::Variable::Variable(int r)
{
  set((real_t)r);
}

ogm::Variable::Variable(string s)
{
  set(s);
}

ogm::Variable::Variable(const char* s)
{
  set(s);
}

ogm::Variable::Variable(std::vector<ogm::Variable> v)
{
  set(v);
}

ogm::Variable::Variable(void* ptr)
{
  set(ptr);
}

ogm::Variable::Variable(const ogm::Variable& other)
{
  set(other);
}

ogm::Variable::~Variable()
{
  cleanup();
}

ogm::Variable& ogm::Variable::set(real_t r)
{
  cleanup();
  hdr = VT_REAL;
  *(real_t*)(&val[0]) = r;

  return *this;
}

ogm::Variable& ogm::Variable::set(string s)
{
  cleanup();
  hdr = VT_STRING;
  *(string**)(&val[0]) = new string(s);
  return *this;
}

ogm::Variable& ogm::Variable::set(const char* s)
{
  set((wchar_t*)s);
}

ogm::Variable& ogm::Variable::set(const std::vector<ogm::Variable>& v)
{
  cleanup();
  hdr = VT_ARRAY;
  *(std::vector<ogm::Variable>**)(&val[0]) = new std::vector<ogm::Variable>(v);
  return *this;
}

ogm::Variable& ogm::Variable::set(void* ptr)
{
  cleanup();
  hdr = VT_PTR;
  *(void**)(&val[0]) = ptr;
  return *this;
}

ogm::Variable& ogm::Variable::set(const ogm::Variable& other)
{
  switch (other.get_type()) {
    case VT_REAL:
      return set(other.get_real());
    case VT_STRING:
      return set(other.get_string());
    case VT_ARRAY:
      return set(other.get_real());
    case VT_PTR:
      return set(other.get_ptr());
  }
}

ogm::Variable& ogm::Variable::operator=(real_t r)
{
  return set(r);
}

ogm::Variable& ogm::Variable::operator=(string s)
{
  return set(s);
}

ogm::Variable& ogm::Variable::operator=(const char* s)
{
  return set(s);
}

ogm::Variable& ogm::Variable::operator=(void* ptr)
{
  return set(ptr);
}

ogm::Variable& ogm::Variable::operator=(const ogm::Variable& other)
{
  return set(other);
}

bool ogm::Variable::operator==(const ogm::Variable& other) const
{
  if (other.get_type() == get_type())
  switch (get_type())
  {
    case VT_REAL:
      return get_real() == other.get_real();
    case VT_STRING:
      return get_string() == other.get_string();
    case VT_ARRAY:
      throw TypeError("cannot compare arrays.");
      return false;
    case VT_PTR:
      return get_ptr() == other.get_ptr();
  }
  return false;
}

bool ogm::Variable::operator>=(const ogm::Variable& other) const
{
  return get_real() >= other.get_real();
}

bool ogm::Variable::operator>(const ogm::Variable& other) const
{
  return get_real() > other.get_real();
}

bool ogm::Variable::operator<=(const ogm::Variable& other) const
{
  return get_real() <= other.get_real();
}

bool ogm::Variable::operator<(const ogm::Variable& other) const
{
  return get_real() < other.get_real();
}

void ogm::Variable::cleanup()
{
  switch (get_type()) {
    case VT_STRING:
      delete(&get_string());
    case VT_ARRAY:
      delete(&get_vector_ref());
  }
  return;
}

real_t ogm::Variable::get_real() const
{
  check_type(VT_REAL);
  return *(real_t*)(&val[0]);
}

const string& ogm::Variable::get_string() const
{
  check_type(VT_STRING);
  return **(string**)(&val[0]);
}

std::vector<ogm::Variable>& ogm::Variable::get_vector_ref()
{
  check_type(VT_ARRAY);
  return **(std::vector<ogm::Variable>**)(&val[0]);
}

const std::vector<ogm::Variable>& ogm::Variable::get_vector_ref() const
{
  check_type(VT_ARRAY);
  return **(const std::vector<ogm::Variable>**)(&val[0]);
}


void* ogm::Variable::get_ptr() const
{
  check_type(VT_PTR);
  return *(void**)(&val[0]);
}

real_t ogm::Variable::operator+(real_t r) const
{
  return get_real() + r;
}

string ogm::Variable::operator+(string s) const
{
  return string(get_string() + s);
}

ogm::Variable ogm::Variable::operator+(const ogm::Variable& other) const
{
  switch (get_type()) {
    case VT_REAL:
      return Variable(get_real() + other.get_real());
    case VT_STRING:
      return Variable(get_string() + other.get_string());
  }
  throw TypeError("cannot add non-real_t / non-string types");
}

ogm::Variable& ogm::Variable::operator+=(real_t r)
{
  set(get_real() + r);
  return *this;
}

ogm::Variable& ogm::Variable::operator+=(string s)
{
  set(get_string() + s);
  return *this;
}

ogm::Variable& ogm::Variable::operator+=(const ogm::Variable& other)
{
  auto altvar = *this + other;
  return (*this = altvar);
}

real_t ogm::Variable::operator-(real_t r) const
{
  return get_real() - r;
}

ogm::Variable ogm::Variable::operator-(const ogm::Variable& other) const
{
  return get_real() - other.get_real();
}

ogm::Variable& ogm::Variable::operator-=(const ogm::Variable& other)
{
  *(real_t*)(&val[0]) -= other.get_real();
  return *this;
}

const Variable& ogm::Variable::operator[](int i) const
{
  return get_vector_ref()[i];
}

const Variable& ogm::Variable::operator[](const ogm::Variable& other) const
{
  return (*this)[(int)other.get_real()];
}

Variable& ogm::Variable::operator[](int i)
{
  return get_vector_ref()[i];
}

Variable& ogm::Variable::operator[](const ogm::Variable& other)
{
  return (*this)[(int)other.get_real()];
}

void ogm::Variable::check_type(VariableType vt) const {
  if (get_type() != vt) {
    std::string s = "Expected type " + std::to_string(vt) + ", was type " + std::to_string(get_type());
    throw TypeError(s);
  }
}