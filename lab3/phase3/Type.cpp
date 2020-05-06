#include <cassert>
#include "tokens.h"
#include "Type.h"


using namespace std;

Type::Type()
    :_declarator(ERROR)
{

}

Type::Type(int specifier, unsigned indirection)
    :_declarator(SCALAR), _specifier(specifier), _indirection(indirection)
{

}

Type::Type(int specifier, unsigned indirection, unsigned length)
    :_declarator(ARRAY), _specifier(specifier), _indirection(indirection), _length(length)
{

}

Type::Type(int specifier, unsigned indirection, Parameters *parameters)
    :_declarator(FUNCTION), _specifier(specifier), _indirection(indirection), _parameters(parameters)
{

}

bool Type::operator !=(const Type &rhs) const {
  return !operator == (rhs);
}

bool Type::operator ==(const Type &rhs) const {
  if (_declarator != rhs._declarator)
    return false;
  if (_declarator == ERROR)
    return true;
  if (_specifier != rhs._specifier)
    return false;
  if (_indirection != rhs._indirection)
    return false;
  if (_declarator==SCALAR)
    return true;
  if (_declarator==ARRAY)
    return _length==rhs._length;
  assert(_declarator==FUNCTION);
  if (_parameters->variadic!=rhs._parameters->variadic)
    return false;
  return _parameters->types==rhs._parameters->types;
}

std::ostream &operator <<(std::ostream &ostr, const Type &type) {
  cout << "Declarator: " << type.declarator() << endl;
  cout << "Specifier: " << type.specifier() << endl;
  cout << "Indirection: " << type.indirection() << endl;
  cout << "Length" << type.length() << endl;
  cout << "Parameters" << type.parameters() << endl;
  return ostr;
}
