/*
 * File:	Type.cpp
 *
 * Description:	This file contains the member function definitions for
 *		types in Simple C.  A type is either a scalar type, an
 *		array type, or a function type.
 *
 *		Note that we simply don't like putting function definitions
 *		in the header file.  The header file is for the interface.
 *		Actually, we prefer opaque pointer types in C where you
 *		don't even get to see what's inside, much less touch it.
 *		But, C++ lets us have value types with access control
 *		instead of just always using pointer types.
 *
 *		Extra functionality:
 *		- equality and inequality operators
 *		- predicate functions such as isArray()
 *		- stream operator
 *		- the error type
 */

# include <cassert>
# include "tokens.h"
# include "Type.h"

using namespace std;


/*
 * Function:	Type::Type (constructor)
 *
 * Description:	Initialize this type as an error type.
 */

Type::Type()
    : _declarator(ERROR)
{
}


/*
 * Function:	Type::Type (constructor)
 *
 * Description:	Initialize this type object as a scalar type.
 */

Type::Type(int specifier, unsigned indirection)
    : _specifier(specifier), _indirection(indirection)
{
    _declarator = SCALAR;
}


/*
 * Function:	Type::Type (constructor)
 *
 * Description:	Initialize this type object as an array type.
 */

Type::Type(int specifier, unsigned indirection, unsigned length)
    : _specifier(specifier), _indirection(indirection), _length(length)
{
    _declarator = ARRAY;
}


/*
 * Function:	Type::Type (constructor)
 *
 * Description:	Initialize this type object as a function type.
 */

Type::Type(int specifier, unsigned indirection, Parameters *parameters)
    : _specifier(specifier), _indirection(indirection), _parameters(parameters)
{
    _declarator = FUNCTION;
}


/*
 * Function:	Type::operator ==
 *
 * Description:	Return whether another type is equal to this type.  The
 *		parameter lists are checked for function types, which C++
 *		makes so easy.  (At least, it makes something easy!)
 */

bool Type::operator ==(const Type &rhs) const
{
    if (_declarator != rhs._declarator)
	return false;

    if (_declarator == ERROR)
	return true;

    if (_specifier != rhs._specifier)
	return false;

    if (_indirection != rhs._indirection)
	return false;

    if (_declarator == SCALAR)
	return true;

    if (_declarator == ARRAY)
	return _length == rhs._length;

    if (_parameters->variadic != rhs._parameters->variadic)
	return false;

    return _parameters->types == rhs._parameters->types;
}


/*
 * Function:	Type::operator !=
 *
 * Description:	Well, what do you think it does?  Why can't the language
 *		generate this function for us?  Because they think we want
 *		it to do something else?  Yeah, like that'd be a good idea.
 */

bool Type::operator !=(const Type &rhs) const
{
    return !operator ==(rhs);
}


/*
 * Function:	Type::isArray
 *
 * Description:	Return whether this type is an array type.
 */

bool Type::isArray() const
{
    return _declarator == ARRAY;
}


/*
 * Function:	Type::isScalar
 *
 * Description:	Return whether this type is a scalar type.
 */

bool Type::isScalar() const
{
    return _declarator == SCALAR;
}


/*
 * Function:	Type::isFunction
 *
 * Description:	Return whether this type is a function type.
 */

bool Type::isFunction() const
{
    return _declarator == FUNCTION;
}


/*
 * Function:	Type::isError
 *
 * Description:	Return whether this type is an error type.
 */

bool Type::isError() const
{
    return _declarator == ERROR;
}


/*
 * Function:	Type::specifier (accessor)
 *
 * Description:	Return the specifier of this type.
 */

int Type::specifier() const
{
    return _specifier;
}


/*
 * Function:	Type::indirection (accessor)
 *
 * Description:	Return the number of levels of indirection of this type.
 */

unsigned Type::indirection() const
{
    return _indirection;
}


/*
 * Function:	Type::length (accessor)
 *
 * Description:	Return the length of this type, which must be an array
 *		type.  Is there a better way than calling assert?  There
 *		certainly isn't an easier way.
 */

unsigned Type::length() const
{
    assert(_declarator == ARRAY);
    return _length;
}


/*
 * Function:	Type::parameters (accessor)
 *
 * Description:	Return the parameters of this type, which must be a
 *		function type.
 */

Parameters *Type::parameters() const
{
    assert(_declarator == FUNCTION);
    return _parameters;
}

Type Type::promote() const {
  if (_specifier==CHAR&&_indirection==0&&_declarator==SCALAR)
    return Type(INT);
  if(_declarator==ARRAY)
    return Type(_specifier, _indirection+1);
  return *this;
}

bool Type::isPredicate() const {
  return _declarator!=FUNCTION && _declarator!=ERROR;
} // return isNumeric() || isPointer();

// SHOULD BE DONE LATER
// bool Type::isCompatibleWith(const Type &that) const {
//   if (isNumeric()&&that.isNumeric()) {
//     return true;
//   }
//   if ((isPredicate()) && (*this==that)) {
//     return true;
//   }
//   return false;
// }

bool Type::isCompatibleWith(const Type &that) const{
  if (isNumeric() && that.isNumeric())
    return true;

  return (isPredicate() && promote() == that.promote());
}

bool Type::isNumeric() const {
  return _indirection==0 && _declarator==SCALAR && (_specifier==INT||_specifier==DOUBLE);
}

bool Type::isPointer() const {
  return _declarator == SCALAR && _indirection > 0;
}

bool Type::isInteger() const {
  return _declarator == SCALAR && _specifier == INT && _indirection == 0;
}

bool Type::isDouble() const {
  return _declarator == SCALAR && _specifier == DOUBLE && _indirection == 0;
}


/*
 * Function:	operator <<
 *
 * Description:	Write a type to the specified output stream.  At least C++
 *		let's us do some cool things.
 */

ostream &operator <<(ostream &ostr, const Type &type)
{
    unsigned i;


    if (type.isError())
	ostr << "error";

    else {
	if (type.specifier() == CHAR)
	    ostr << "char";
	else if (type.specifier() == INT)
	    ostr << "int";
	else if (type.specifier() == DOUBLE)
	    ostr << "double";
	else
	    ostr << "-unknown specifier-";

	if (type.indirection() > 0)
	    ostr << " " << string(type.indirection(), '*');

	if (type.isArray())
	    ostr << "[" << type.length() << "]";

	else if (type.isFunction()) {
	    ostr << "(";

	    if (type.parameters() != nullptr) {
		if (type.parameters()->types.size() == 0)
		    ostr << "void";
		else
		    for (i = 0; i < type.parameters()->types.size(); i ++) {
			if (i > 0)
			    ostr << ", ";
			ostr << type.parameters()->types[i];
		    }

		if (type.parameters()->variadic)
		    ostr << ", ...";
	    }

	    ostr << ")";
	}
    }

    return ostr;
}
