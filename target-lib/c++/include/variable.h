#ifndef OGM_VARIABLE_H
#define OGM_VARIABLE_H

#include <string>
#include <vector>

namespace ogm
{ 
  typedef double real;
  typedef unsigned char byte;
  
  enum VariableType {
    VT_REAL, // real number
    VT_STRING, // string
    VT_ARRAY, // untyped array
    VT_PTR // other data
  }
  
  class Variable {
  public:
    Variable(real);
    Variable(std::string);
    Variable(std::vector<Variable>);
    Variable(void*);
    Variable(const Variable&);
    ~Variable();
    
    Variable& set(real);
    Variable& set(std::string);
    Variable& set(const std::vector<Variable>&);
    Variable& set(void*);
    Variable& set(const Variable&);
    
    Variable& operator=(real);
    Variable& operator=(std::string);
    Variable& operator=(void*);
    Variable& operator=(const Variable&);
    
    real operator+(real);
    std::string operator+(std::string);
    Variable operator+(const Variable&);
    Variable& operator+=(real);
    Variable& operator+=(std::string);
    Variable& operator+=(const Variable&);
    
    Variable& operator[](int i);
    Variable& operator[](const Variable&);
    
    inline VariableType get_type() {return (VariableType)hdr;}
    
    real get_real() const;
    std::string get_string() const;
    std::vector<Variable>& get_vector_ref() const;
    void* get_ptr() const;
    
  private:
    void cleanup();
    void check_type(VariableType);

    byte hdr = (byte)VT_REAL;
    byte val[8];
  };
}

#endif /*OGM_VARIABLE_H*/