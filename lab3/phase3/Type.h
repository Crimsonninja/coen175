/*
 * File:  types.h
 *
 * Description: This file contains the declarations of the Type class
 */

#ifndef TYPE_H
#define TYPE_H
#include <vector>
#include <iostream>

struct Parameters {
  bool variadic;
  std::vector<class Type> types;
};

class Type {
  enum {ARRAY, ERROR, FUNCTION, SCALAR};
  short _declarator, _specifier;
  unsigned _indirection;

  unsigned _length;
  Parameters *_parameters;

public:
  Type();
  Type(int specifier, unsigned indirection = 0); // Scalar
  Type(int specifier, unsigned indirection, unsigned length); // Array
  Type(int specifier, unsigned indirection, Parameters *parameters); //Function

  bool operator ==(const Type &rhs) const;
  bool operator !=(const Type &rhs) const;

  int declarator() const {return _declarator;}
  int specifier() const {return _specifier;}

  unsigned indirection() const {return _indirection;}
  unsigned length() const {return _length;}

  Parameters* parameters() const {return _parameters;}


  bool isArray() const {return _declarator==ARRAY;}
  bool isError() const {return _declarator==ERROR;}
  bool isFunction() const {return _declarator==FUNCTION;}
  bool isScalar() const {return _declarator==SCALAR;}
};

std::ostream &operator <<(std::ostream &ostr, const Type &type);

#endif /* TYPE_H */
