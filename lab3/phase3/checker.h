/*
 * File:  checker.h
 *
 * Description: This file contains the public function and variable
 *    declarations for the lexical analyzer for Simple C.
 */

# ifndef CHECKER_H
# define CHECKER_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <unordered_set>
#include "Type.h"
#include "Symbol.h"
#include "Scope.h"
#include "lexer.h"

typedef std::string string;



Scope * openScope();
Scope * closeScope();
Symbol * declareFunction(const string &name, const Type &type);
Symbol * declareVariable(const string &name, const Type &type);
Symbol * defineFunction(const string &name, const Type &type);
Symbol * checkIdentifier(const string &name);

# endif /* CHECKER_H */
