#ifndef OGM_VARIABLE_H
#define OGM_VARIABLE_H

#include <string>
#include <vector>

namespace ogm
{ 
  typedef unsigned int VariableID;
  typedef double real;
  typedef unsigned char byte;
  typedef std::wstring string;
  
  enum VariableType {
    VT_REAL, // real number
    VT_STRING, // string
    VT_ARRAY, // untyped array
    VT_PTR // other data
  };
  
  class Variable {
  public:
    Variable(real);
    Variable(string);
    Variable(std::vector<Variable>);
    Variable(void*);
    Variable(const Variable&);
    ~Variable();
    
    Variable& set(real);
    Variable& set(string);
    Variable& set(const std::vector<Variable>&);
    Variable& set(void*);
    Variable& set(const Variable&);
    
    Variable& operator=(real);
    Variable& operator=(string);
    Variable& operator=(void*);
    Variable& operator=(const Variable&);
    
    bool operator==(const Variable&);
    bool operator>=(const Variable&);
    bool operator>(const Variable&);
    bool operator<=(const Variable&);
    bool operator<(const Variable&);
    
    real operator+(real);
    string operator+(string);
    Variable operator+(const Variable&);
    Variable& operator+=(real);
    Variable& operator+=(string);
    Variable& operator+=(const Variable&);
    
    real operator-(real);
    Variable operator-(const Variable&);
    Variable& operator-=(const Variable&);
    
    Variable& operator[](int i);
    Variable& operator[](const Variable&);
    
    inline VariableType get_type() {return (VariableType)hdr;}
    
    real get_real() const;
    string get_string() const;
    std::vector<Variable>& get_vector_ref() const;
    void* get_ptr() const;
    
  private:
    void cleanup();
    void check_type(VariableType);

    byte hdr = (byte)VT_REAL;
    byte val[8];
  };
  
  typedef Variable var;
}

#endif /*OGM_VARIABLE_H*/