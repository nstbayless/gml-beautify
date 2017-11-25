#include "variable.h"
#include "error.h"

using namespace ogm;

ogm::Variable::Variable(real r)
{
  set(r);
}

ogm::Variable::Variable(std::string s)
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

ogm::Variable& ogm::Variable::set(real r)
{
  cleanup();
  hdr = VT_REAL;
  *(real*)(&val[0]) = r;

  return *this;
}

ogm::Variable& ogm::Variable::set(std::string s)
{
  cleanup();
  hdr = VT_STRING;
  *(std::string**)(&val[0]) = new std::string(s);
  return *this;
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

ogm::Variable& ogm::Variable::operator=(real r)
{
  return set(r);
}

ogm::Variable& ogm::Variable::operator=(std::string s)
{
  return set(s);
}

ogm::Variable& ogm::Variable::operator=(void* ptr)
{
  return set(ptr);
}

ogm::Variable& ogm::Variable::operator=(const Variable& other)
{
  return set(other);
}

void ogm::Variable::cleanup()
{
  switch (get_type()) {
    case VT_STRING:
      delete(&get_string());
    case VT_ARRAY:
      delete(&get_array());
  }
  return;
}

real ogm::Variable::get_real() const
{
  check_type(VT_REAL);
  return *(real*)(&val[0]);
}

std::string& ogm::Variable::get_string() const
{
  check_type(VT_STRING);
  return **(std::string**)(&val[0]);
}

std::vector<ogm::Variable>& ogm::Variable::get_vector_ref() const
{
  check_type(VT_ARRAY);
  return **(std::vector<ogm::Variable>**)(&val[0]);
}

void* ogm::Variable::get_ptr() const
{
  check_type(VT_PTR);
  return *(void**)(&val[0]);
}

real ogm::Variable::operator+(real r)
{
  return get_real() + r;
}

std::string ogm::Variable::operator+(std::string s)
{
  return string(get_string() + s)
}

ogm::Variable ogm::Variable::operator+(const Variable& other)
{
  switch (get_type()) {
    case VT_REAL:
      return Variable(get_real() + other.get_real());
    case VT_STRING:
      return Variable(get_string() + other.get_string());
  }
}

ogm::Variable& ogm::Variable::operator+=(real r)
{
  set(get_real() + r);
  return *this;
}

ogm::Variable& ogm::Variable::operator+=(std::string s)
{
  set(get_string() + s);
  return *this;
}

ogm::Variable& ogm::Variable::operator+=(const Variable& other)
{
  auto altvar = *this + other;
  return (*this = altvar);
}

Variable& operator[](int i)
{
  return get_array_ref()[i];
}

Variable& operator[](const Variable& other)
{
  return get_array_ref()[(int)other.get_real()];
}

void check_type(VariableType vt) {
  if (get_type() != vt)
    throw TypeError("Expected type " + std::to_string(vt)+ ", was type " + std::string(get_type()));
}