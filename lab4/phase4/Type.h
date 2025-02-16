/*
 * File:	Type.h
 *
 * Description:	This file contains the class definition for types in Simple
 *		C.  A type is either a scalar type, an array type, or a
 *		function type.  These types include a specifier and the
 *		number of levels of indirection.  Array types also have a
 *		length, and function types also have a parameter list.
 *		An error type is also supported for use in undeclared
 *		identifiers and the results of type checking.
 *
 *		No subclassing is used to avoid the problem of object
 *		slicing (since we'll be treating types as value types) and
 *		the proliferation of small member functions.
 *
 *		As we've designed them, types are essentially immutable,
 *		since we haven't included any mutators.  In practice, we'll
 *		be creating new types rather than changing existing types.
 */

# ifndef TYPE_H
# define TYPE_H
# include <vector>
# include <ostream>

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
    Type(int specifier, unsigned indirection = 0);
    Type(int specifier, unsigned indirection, unsigned length);
    Type(int specifier, unsigned indirection, Parameters *parameters);

    bool operator ==(const Type &rhs) const;
    bool operator !=(const Type &rhs) const;

    bool isArray() const;
    bool isScalar() const;
    bool isFunction() const;
    bool isError() const;

    int specifier() const;
    unsigned indirection() const;
    unsigned length() const;
    Parameters *parameters() const;

    Type promote() const;
    bool isPredicate() const;
    bool isCompatibleWith(const Type &that) const;
    bool isNumeric() const;
    bool isPointer() const;
    bool isInteger() const;
    bool isDouble() const;
};

std::ostream &operator <<(std::ostream &ostr, const Type &type);

# endif /* TYPE_H */
