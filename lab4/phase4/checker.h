/*
 * File:	checker.h
 *
 * Description:	This file contains the public function declarations for the
 *		semantic checker for Simple C.
 */

# ifndef CHECKER_H
# define CHECKER_H
# include "Scope.h"

typedef std::string string;
typedef std::vector<Type> Types;

Scope *openScope();
Scope *closeScope();

Symbol *defineFunction(const std::string &name, const Type &type);
Symbol *declareFunction(const std::string &name, const Type &type);
Symbol *declareVariable(const std::string &name, const Type &type);
Symbol *checkIdentifier(const std::string &name);

Type checkLogical(const Type &left, const Type &right, const string &op);
Type checkEquality(const Type &left, const Type &right, const string &op);
Type checkRelational(const Type &left, const Type &right, const string &op);
Type checkAdd(const Type &left, const Type &right);
Type checkMinus(const Type &left, const Type &right);
Type checkMultiply(const Type &left, const Type &right);
Type checkDivide(const Type &left, const Type &right);
Type checkRemainder(const Type &left, const Type &right);
Type checkNot(const Type &right);
Type checkNegation(const Type &right);
Type checkDereference(const Type &right);
Type checkAddress(const Type &right, const bool &lvalue);
Type checkSizeOf(const Type &right);
Type checkCast(const Type &left, const Type &right);
Type checkIndex(const Type &left, const Type &right);
Type checkIncrement(const Type &left, const bool &lvalue);
Type checkDecrement(const Type &left, const bool &lvalue);
Type checkCall(const Type &left, Types &types);

Type checkAssignment(const Type &left, const Type &right, const bool &lvalue);
void checkReturn(const Type &returned, const Type &function_type);
Type checkWhileIfFor(const Type &some_condition);
void checkBreak(const int loopDepth);

# endif /* CHECKER_H */
