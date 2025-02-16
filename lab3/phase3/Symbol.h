/*
 * File:  Symbol.h
 *
 * Description: This file contains the declarations of the Scope class
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include "Type.h"


class Symbol {
  typedef std::string string;

  string _name;
  Type _type;

public:
  Symbol();
  Symbol(const string& name, const Type& type);

  const string name() const {return _name;}
  const Type& type() const {return _type;}

};

#endif /* SYMBOL_H */
