/*
 * File:  scope.h
 *
 * Description: This file contains the declarations of the Scope class
 */

#ifndef SCOPE_H
#define SCOPE_H

#include "Symbol.h"
#include <vector>
#include <string>
#include "lexer.h"

typedef std::vector<Symbol *> Symbols;

class Scope {
  typedef std::string string;

  Scope *_enclosing;
  Symbols _symbols;

public:
  Scope(Scope *enclosing = nullptr);

  void insert(Symbol *symbol);
  Symbol *find(const string &name) const;
  Symbol *lookup(const string &name) const;

  Scope *enclosing() const {return _enclosing;}
  const Symbols &symbols() const {return _symbols;}
};

#endif /* SCOPE_H */
