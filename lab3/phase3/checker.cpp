#include "checker.h"

using namespace std;

Scope * globalScope=nullptr;
Scope * currentScope;

unordered_set <string> definedFunctions;

static string conflicting_type = "conflicting types for '%s'";
static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string undeclared = "'%s' undeclared";


Scope * openScope() {
  // cout << "Open scope (" << scope << ")" << endl;
  if (globalScope == nullptr) {
    globalScope = new Scope();
    currentScope = globalScope;
  }
  else {
    Scope * blah = new Scope(currentScope);
    currentScope = blah;
  }
  return currentScope;
}

Scope * closeScope() {
  Scope * freed = currentScope;
  currentScope = currentScope->enclosing();
  return freed;
}

Symbol* declareFunction(const string &name, const Type &type) {
  Symbol *sym = globalScope->find(name);
  if (sym==nullptr) { // function not found in global scope
    sym = new Symbol(name, type);
    globalScope->insert(sym);
  }
  else if (type!= sym->type()) {
    report(conflicting_type, name);
  }
  return sym;
}

Symbol *declareVariable(const string &name, const Type &type) {
  Symbol *sym = currentScope->find(name);
  if (sym==nullptr) {
    sym = new Symbol(name, type);
    currentScope->insert(sym);
  }
  else if (currentScope->enclosing()!=nullptr) {
    report(redeclared, name);
  }
  else if (type != sym->type()) {
    report(conflicting_type, name);
  }
  return sym;
}

Symbol * defineFunction(const string &name, const Type &type) {
  // cout << "FUNC DEFINITION" << endl;
  if (definedFunctions.count(name)) { // found in set of defined functions
    report(redefined, name);
    return globalScope->find(name);
  } else {  // not found in set of defined functions
    definedFunctions.insert(name);
    return declareFunction(name, type);
  }
}

Symbol * checkIdentifier(const string &name) {
  Symbol *sym = currentScope->lookup(name);
  if (sym==nullptr) {
    sym = new Symbol(name, Type());
    currentScope->insert(sym);
    report(undeclared, name);
  }
  return sym;
}
