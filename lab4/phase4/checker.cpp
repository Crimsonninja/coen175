/*
 * File:  checker.cpp
 *
 * Description: This file contains the public and private function and
 *    variable definitions for the semantic checker for Simple C.
 *
 *    If a symbol is redeclared, the redeclaration is discarded
 *    and the original declaration is retained.
 *
 *    Extra functionality:
 *    - inserting an undeclared symbol with the error type
 */

# include <iostream>
# include <unordered_set>
# include "lexer.h"
# include "checker.h"
# include "tokens.h"
# include "Symbol.h"
# include "Scope.h"
# include "Type.h"


using namespace std;

static unordered_set<string> defined;
static Scope *outermost, *toplevel;
static const Type error;

static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string conflicting = "conflicting types for '%s'";
static string undeclared = "'%s' undeclared";

static string breaking = "break statement not within loop";
static string return_type = "invalid return type";
static string type_expression = "invalid type for test expression";
static string lvalue_expression = "lvalue required in expression";
static string invalid_binary_operands = "invalid operands to binary %s";
static string invalid_unary_operands = "invalid operand to unary %s";
static string invalid_operand_sizeof = "invalid operand in sizeof expression";
static string invalid_operand_cast = "invalid operand in cast expression";
static string object_not_function = "called object is not a function";
static string invalid_arguments = "invalid arguments to called function";



static Type integer(INT);


/*
 * Function:  openScope
 *
 * Description: Create a scope and make it the new top-level scope.
 */

Scope *openScope() {
  toplevel = new Scope(toplevel);

  if (outermost == nullptr)
    outermost = toplevel;

  return toplevel;
}


/*
 * Function:  closeScope
 *
 * Description: Remove the top-level scope, and make its enclosing scope
 *    the new top-level scope.
 */

Scope *closeScope() {
  Scope *old = toplevel;
  toplevel = toplevel->enclosing();
  return old;
}


/*
 * Function:  defineFunction
 *
 * Description: Define a function with the specified NAME and TYPE.  A
 *    function is always defined in the outermost scope.
 */

Symbol *defineFunction(const string &name, const Type &type) {
  if (defined.count(name) > 0) {
    report(redefined, name);
    return outermost->find(name);
  }
  defined.insert(name);
  return declareFunction(name, type);
}


/*
 * Function:  declareFunction
 *
 * Description: Declare a function with the specified NAME and TYPE.  A
 *    function is always declared in the outermost scope.  Any
 *    redeclaration is discarded.
 */

Symbol *declareFunction(const string &name, const Type &type)
{
    cout << name << ": " << type << endl;
    Symbol *symbol = outermost->find(name);

    if (symbol == nullptr) {
  symbol = new Symbol(name, type);
  outermost->insert(symbol);

    } else if (type != symbol->type()) {
  report(conflicting, name);
  delete type.parameters();

    } else
  delete type.parameters();

    return symbol;
}


/*
 * Function:  declareVariable
 *
 * Description: Declare a variable with the specified NAME and TYPE.  Any
 *    redeclaration is discarded.
 */

Symbol *declareVariable(const string &name, const Type &type)
{
    cout << name << ": " << type << endl;
    Symbol *symbol = toplevel->find(name);

    if (symbol == nullptr) {
  symbol = new Symbol(name, type);
  toplevel->insert(symbol);

    } else if (outermost != toplevel)
  report(redeclared, name);

    else if (type != symbol->type())
  report(conflicting, name);

    return symbol;
}


/*
 * Function:  checkIdentifier
 *
 * Description: Check if NAME is declared.  If it is undeclared, then
 *    declare it as having the error type in order to eliminate
 *    future error messages.
 */

Symbol *checkIdentifier(const string &name)
{
    Symbol *symbol = toplevel->lookup(name);

    if (symbol == nullptr) {
  report(undeclared, name);
  symbol = new Symbol(name, error);
  toplevel->insert(symbol);
    }

    return symbol;
}

Type checkLogical(const Type &left, const Type &right, const string &op) {
  if (left.isError() || right.isError())
    return error;
  Type leftProm = left.promote();
  Type rightProm = right.promote();
  if (leftProm.isPredicate() && rightProm.isPredicate())
      return integer;
  report(invalid_binary_operands, op);
  return error;
}

Type checkEquality(const Type &left, const Type &right, const string &op) {
  if (left.isError() || right.isError())
    return error;
  Type leftProm = left.promote();
  Type rightProm = right.promote();
  if (leftProm.isCompatibleWith(rightProm)) {
      return integer;
  }
  report(invalid_binary_operands, op);
  return error;
}

Type checkRelational(const Type &left, const Type &right, const string &op) {
  if (left.isError() || right.isError()) {
    return error;
  }
  Type leftProm = left.promote();
  Type rightProm = right.promote();
  if (leftProm.isCompatibleWith(rightProm)) {
      return integer;
  }
  report(invalid_binary_operands, op);
  return error;
}

// May need to check for Errors
Type checkAdd(const Type &left, const Type &right) {
  if (left.isError() || right.isError())
    return error;
  Type leftProm = left.promote();
  Type rightProm = right.promote();

  if (leftProm.isNumeric() && rightProm.isNumeric()) {
    if (leftProm.isDouble() || rightProm.isDouble()) {
      return Type(DOUBLE);
    }
    else {
      return integer;
    }
  }
  if (leftProm.isPointer() && rightProm.isInteger())
    return Type(leftProm.specifier(), leftProm.indirection());

  if (leftProm.isInteger() && rightProm.isPointer())
    return Type(rightProm.specifier(), rightProm.indirection());

  report(invalid_binary_operands,"+");
  return error;
}

Type checkMinus(const Type &left, const Type &right) {
  if (left.isError() || right.isError())
    return error;
  Type leftProm = left.promote();
  Type rightProm = right.promote();

  if (leftProm.isNumeric() && rightProm.isNumeric()) {
    if (leftProm.isDouble() || rightProm.isDouble()) {
      return Type(DOUBLE);
    }
    else {
      return integer;
    }
  }
  if (leftProm.isPointer() && rightProm.isInteger()) {
    return Type(leftProm.specifier(), leftProm.indirection());
  }

  if (leftProm.isPointer() && leftProm==rightProm) {
    return integer;
  }
  report(invalid_binary_operands,"-");
  return error;
}

Type checkMultiply(const Type &left, const Type &right) {
  if (left.isError() || right.isError())
    return error;
  Type leftProm = left.promote();
  Type rightProm = right.promote();

  if (leftProm.isNumeric() && rightProm.isNumeric()) {
    if (leftProm.isDouble() || rightProm.isDouble())
      return Type(DOUBLE);
    else
      return integer;
  }
  report(invalid_binary_operands,"*");
  return error;
}

Type checkDivide(const Type &left, const Type &right) {
  if (left.isError() || right.isError())
    return error;
  Type leftProm = left.promote();
  Type rightProm = right.promote();

  if (leftProm.isNumeric() && rightProm.isNumeric()) {
    if (leftProm.isDouble() || rightProm.isDouble())
      return Type(DOUBLE);
    else
      return integer;
  }
  report(invalid_binary_operands,"/");
  return error;
}

Type checkRemainder(const Type &left, const Type &right) {
  if (left.isError() || right.isError())
    return error;
  Type leftProm = left.promote();
  Type rightProm = right.promote();

  if (leftProm.isInteger() && rightProm.isInteger()) {
    return integer;
  }
  report(invalid_binary_operands, "%");
  return error;
}

Type checkNot(const Type &right) {
  if (right.isError())
    return error;
  if (right.isPredicate())
    return integer;
  report(invalid_unary_operands, "!");
  return error;
}

Type checkNegation(const Type &right) {
  if (right.isError())
    return error;
  if (right.isNumeric()) {
    if (right == right.promote())
      return Type(right.specifier());
    else {
      return error;
    }
  }
  else {
    report(invalid_unary_operands,"-");
    return error;
  }
}

Type checkDereference(const Type &right) {
  if (right.isError())
    return error;
  Type rightProm = right.promote();

  if (rightProm.isPointer()) {
    return Type(rightProm.specifier(), rightProm.indirection()-1);
  }
  report(invalid_unary_operands,"*");
  return error;
}

Type checkAddress(const Type &right, const bool &lvalue) {
  if (right.isError())
    return error;
  if (lvalue==true)
    return Type(right.specifier(), right.indirection()+1);
  report(lvalue_expression);
  return error;
}

Type checkSizeOf(const Type &right) {
  if (right.isError())
    return error;
  if (!right.isFunction())
    return integer;
  report(invalid_operand_sizeof);
  return error;
}

Type checkCast(const Type &result, const Type &operand) {
  if (result.isError() || operand.isError())
    return error;
  if (result.isNumeric() && operand.isNumeric())
    return result;
  if (result.isPointer() && operand.isPointer())
    return result;
  if ((result.isInteger() && operand.isPointer()) || (result.isPointer() && operand.isInteger()))
    return result;
  report(invalid_operand_cast);
  return error;
}

Type checkIndex(const Type &left, const Type &right) {
  if (left.isError() || right.isError())
    return error;
  Type leftProm = left.promote();
  Type rightProm = right.promote();

  if(leftProm.isPointer() && rightProm.isInteger())
    return Type(leftProm.specifier(), leftProm.indirection()-1);
  report(invalid_binary_operands, "[]");
  return error;
}

Type checkIncrement(const Type &left, const bool &lvalue) {
  if (left.isError())
    return error;
  if (lvalue==true) {
    return Type(left.specifier(), left.indirection());
  }
  report(lvalue_expression);
  return error;
}

Type checkDecrement(const Type &left, const bool &lvalue) {
  if (left.isError())
    return error;
  if (lvalue==true) {
    return Type(left.specifier(), left.indirection());
  }
  report(lvalue_expression);
  return error;
}

Type checkCall(const Type &left, Types &types) {
  if (left.isError())
    return error;
  if (left.isFunction()) {
    if (left.parameters()->variadic == true) {
      if (types.size() >= (left.parameters()->types).size()) {
        for (unsigned int i = 0; i < (left.parameters()->types).size(); ++i) {
          Type lprom = left.parameters()->types[i].promote();
          Type rprom = types[i].promote();
          if (!lprom.isCompatibleWith(rprom)) {
            report(invalid_arguments);
            return error;
          }
        }
      }
      else {
        report(invalid_arguments);
        return error;
      }
    }
    else {
      if (types.size() == (left.parameters()->types).size()) {
        for (unsigned int i = 0; i < (left.parameters()->types).size(); ++i) {
          Type lprom = left.parameters()->types[i].promote();
          Type rprom = types[i].promote();
          if (!lprom.isCompatibleWith(rprom)) {
            report(invalid_arguments);
            return error;
          }
        }
      }
      else {
        report(invalid_arguments);
        return error;
      }
    }
  return Type(left.specifier(), left.indirection());
  }
  else {
    report(object_not_function);
    return error;
  }
}

Type checkAssignment(const Type &left, const Type &right, const bool &lvalue) {
  if (left.isError() || right.isError())
    return error;
  if (lvalue==false) {
    // report("Lvalue is false: checkAssignment");
    report(lvalue_expression);
    return error;
  }
  // Need to promote
  Type leftProm = left.promote();
  Type rightProm = right.promote();
  if (leftProm.isCompatibleWith(rightProm)) {
    return left;
  }
  // report("Got here");
  report(invalid_binary_operands,"=");
  return error;
}

Type checkWhileIfFor(const Type &some_condition) {
  if (some_condition.isError())
    return error;
  if (!some_condition.isPredicate())
    report(type_expression);
    return error;
  return some_condition;
}

void checkBreak(const int loopDepth) {
  if (loopDepth <= 0) {
    report(breaking);
  }
}

void checkReturn(const Type &returned, const Type &function_type) {
  Type returnedProm = returned.promote();
  Type function_type_prom = function_type.promote();
  if (!returnedProm.isCompatibleWith(function_type_prom)) {
    report(return_type);
  }
}
