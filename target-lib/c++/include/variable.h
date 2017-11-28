#ifndef OGM_VARIABLE_H
#define OGM_VARIABLE_H

#include <string>
#include <vector>

namespace ogm
{ 
  typedef unsigned int VariableID;
  typedef double real_t;
  typedef unsigned char byte;
  typedef char char_t;
  typedef std::string string_t;
  
  enum VariableType {
    VT_REAL, // real number
    VT_STRING, // string
    VT_ARRAY, // untyped array
    VT_PTR // other data
  };
  
  class Variable {
  public:
    Variable(real_t);
    Variable(int);
    Variable(string_t);
    Variable(const char*);
    Variable(std::vector<Variable>);
    Variable(void*);
    Variable(const Variable&);
    ~Variable();
    
    Variable& set(real_t);
    Variable& set(string_t);
    Variable& set(const char*);
    Variable& set(const std::vector<Variable>&);
    Variable& set(void*);
    Variable& set(const Variable&);
    
    Variable& operator=(real_t);
    Variable& operator=(string_t);
    Variable& operator=(const char*);
    Variable& operator=(void*);
    Variable& operator=(const Variable&);
    
    bool operator==(const Variable&) const;
    bool operator>=(const Variable&) const;
    bool operator>(const Variable&) const;
    bool operator<=(const Variable&) const;
    bool operator<(const Variable&) const;
    
    real_t operator+(real_t) const;
    string_t operator+(string_t) const;
    Variable operator+(const Variable&) const;
    Variable& operator+=(real_t);
    Variable& operator+=(string_t);
    Variable& operator+=(const Variable&);
    
    real_t operator-(real_t) const ;
    Variable operator-(const Variable&) const;
    Variable& operator-=(const Variable&);
    
    const Variable& operator[](int i) const;
    const Variable& operator[](const Variable&) const;
    Variable& operator[](int i);
    Variable& operator[](const Variable&);
    
    inline VariableType get_type() const {return (VariableType)hdr;}
    
    real_t get_real() const;
    const string_t get_string() const;
    const std::vector<Variable>& get_vector_ref() const;
    std::vector<Variable>& get_vector_ref();
    void* get_ptr() const;
    
  private:
    void cleanup();
    void check_type(VariableType) const;

    byte hdr = (byte)VT_REAL;
    byte val[8];
  };
  
  typedef Variable var;
}

#endif /*OGM_VARIABLE_H*/