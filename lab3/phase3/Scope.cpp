#include "Scope.h"

using namespace std;

Scope::Scope(Scope *enclosing)
      :_enclosing(enclosing)
{}

void Scope::insert(Symbol *symbol) {
  _symbols.push_back(symbol);
}

Symbol * Scope::find(const string &name) const {
  for (unsigned int i = 0; i < _symbols.size(); ++i) {
    if (_symbols[i]->name() == name) {
      return _symbols[i];
    }
  }
  return nullptr;
}

Symbol *Scope::lookup(const string &name) const {
  Symbol * found = find(name);
  if (found!=nullptr){
    return found;
  }
  else {
    Scope * currentScope = _enclosing;
    while(currentScope!=nullptr) {
      found = currentScope->find(name);
      if (found!=nullptr) {
        return found;
      }
      currentScope = currentScope->enclosing();
    }
    return nullptr;
  }
}

