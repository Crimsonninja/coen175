/*
 * File:	Tree.cpp
 *
 * Description:	This file contains the constructor and accessor member
 *		function definitions for abstract syntax trees in Simple C.
 *
 *		The tree is actually built during semantic checking, where
 *		type information is readily available.  Any simplifications
 *		or canonicalizations of the tree are performed when it is
 *		constructed.
 *
 *		The functions here are just the constructors and accessors.
 *		The storage allocation and code generation functions are
 *		located elsewhere.  Most of these functions don't do
 *		anything interesting, and could easily be put in the header
 *		file, but we don't like to do that.
 *
 *		Extra functionality:
 *		- everything (it is optional to construct an AST)
 */

# include "Tree.h"
# include "tokens.h"
# include <sstream>
# include <cstdlib>

using namespace std;


/*
 * Function:	Expression::Expression (constructor)
 *
 * Description:	Initialize the expression object to not be an lvalue and to
 *		have the specified type.
 */

Expression::Expression(const Type &type)
    : _type(type), _lvalue(false)
{
}


/*
 * Function:	Expression::type (accessor)
 *
 * Description:	Return the type of this expression.
 */

const Type &Expression::type() const
{
    return _type;
}


/*
 * Function:	Expression::lvalue (accessor)
 *
 * Description:	Return whether this expression is an lvalue.
 */

bool Expression::lvalue() const
{
    return _lvalue;
}


/*
 * Function:	Binary::Binary (constructor)
 *
 * Description:	Initialize this expression as a binary operator with the
 *		specified children.
 */

Binary::Binary(Expression *left, Expression *right, const Type &type)
    : Expression(type), _left(left), _right(right)
{
}


/*
 * Function:	Unary::Unary (constructor)
 *
 * Description:	Initialize this expression as a unary operator with the
 *		specified child.
 */

Unary::Unary(Expression *expr, const Type &type)
    : Expression(type), _expr(expr)
{
}


/*
 * Function:	String::String (constructor)
 *
 * Description:	Initialize this string literal.
 */

String::String(const string &value)
    : Expression(Type(CHAR, 0, value.size() + 1)), _value(value)
{
}


/*
 * Function:	String::value (accessor)
 *
 * Description:	Return the value of this string.
 */

const string &String::value() const
{
    return _value;
}


/*
 * Function:	Identifier::Identifier (constructor)
 *
 * Description:	Initialize this identifier object.  An identifier is an
 *		lvalue if its type is a scalar type.
 */

Identifier::Identifier(const Symbol *symbol)
    : Expression(symbol->type()), _symbol(symbol)
{
    _lvalue = symbol->type().isScalar();
}


/*
 * Function:	Identifier::symbol (accessor)
 *
 * Description:	Return the symbol of this identifier.
 */

const Symbol *Identifier::symbol() const
{
    return _symbol;
}


/*
 * Function:	Integer::Integer (constructor)
 *
 * Description:	Initialize an integer literal, which always has type int.
 */

Integer::Integer(unsigned value)
    : Expression(Type(INT))
{
    stringstream ss;

    ss << value;
    _value = ss.str();
}


/*
 * Function:	Integer::Integer (constructor)
 *
 * Description:	Initialize an integer literal, which always has type int.
 */

Integer::Integer(const string &value)
    : Expression(Type(INT)), _value(value)
{
}


/*
 * Function:	Integer::value (accessor)
 *
 * Description:	Return the value of this integer.
 */

const string &Integer::value() const
{
    return _value;
}


/*
 * Function:	Real::Real (constructor)
 *
 * Description:	Initialize a real literal, which always has type double.
 */

Real::Real(double value)
    : Expression(Type(DOUBLE))
{
    stringstream ss;

    ss << value;
    _value = ss.str();
}


/*
 * Function:	Real::Real (constructor)
 *
 * Description:	Initialize a real literal, which always has type double.
 */

Real::Real(const string &value)
    : Expression(Type(DOUBLE)), _value(value)
{
}


/*
 * Function:	Real::value (accessor)
 *
 * Description:	Return the value of this real.
 */

const string &Real::value() const
{
    return _value;
}


/*
 * Function:	Call::Call (constructor)
 *
 * Description:	Initialize a function call expression.
 */

Call::Call(const Symbol *id, const Expressions &args, const Type &type)
    : Expression(type), _id(id), _args(args)
{
}


/*
 * Function:	Not::Not (constructor)
 *
 * Description:	Initialize a logical negation expression.
 */

Not::Not(Expression *expr, const Type &type)
    : Unary(expr, type)
{
}


/*
 * Function:	Negate::Negate (constructor)
 *
 * Description:	Initialize an arithmetic negation expression.
 */

Negate::Negate(Expression *expr, const Type &type)
    : Unary(expr, type)
{
}


/*
 * Function:	Dereference::Dereference (constructor)
 *
 * Description:	Initialize a dereference expression.
 */

Dereference::Dereference(Expression *expr, const Type &type)
    : Unary(expr, type)
{
    _lvalue = true;
}


/*
 * Function:	Address::Address (constructor)
 *
 * Description:	Initialize an address expression.
 */

Address::Address(Expression *expr, const Type &type)
    : Unary(expr, type)
{
}


/*
 * Function:	Increment::Increment (constructor)
 *
 * Description:	Initialize an increment expression.
 */

Increment::Increment(Expression *expr, const Type &type)
    : Unary(expr, type), scale(0)
{
}


/*
 * Function:	Decrement::Decrement (constructor)
 *
 * Description:	Initialize an decrement expression.
 */

Decrement::Decrement(Expression *expr, const Type &type)
    : Unary(expr, type), scale(0)
{
}


/*
 * Function:	Cast::Cast (constructor)
 *
 * Description:	Initialize a cast expression.
 */

Cast::Cast(const Type &type, Expression *expr)
    : Unary(expr, type)
{
}


/*
 * Function:	Multiply::Multiply (constructor)
 *
 * Description:	Initialize a multiplication expression.
 */

Multiply::Multiply(Expression *left, Expression *right, const Type &type)
    : Binary(left, right, type)
{
}


/*
 * Function:	Divide::Divide (constructor)
 *
 * Description:	Initialize a division expression.
 */

Divide::Divide(Expression *left, Expression *right, const Type &type)
    : Binary(left, right, type)
{
}


/*
 * Function:	Remainder::Remainder (constructor)
 *
 * Description:	Initialize a remainder expression.
 */

Remainder::Remainder(Expression *left, Expression *right, const Type &type)
    : Binary(left, right, type)
{
}


/*
 * Function:	Add::Add (constructor)
 *
 * Description:	Initialize an addition expression.
 */

Add::Add(Expression *left, Expression *right, const Type &type)
    : Binary(left, right, type), scaleLeft(0), scaleRight(0)
{
}


/*
 * Function:	Subtract::Subtract (constructor)
 *
 * Description:	Initialize a subtraction expression.
 */

Subtract::Subtract(Expression *left, Expression *right, const Type &type)
    : Binary(left, right, type), scaleResult(0), scaleRight(0)
{
}


/*
 * Function:	LessThan::LessThan (constructor)
 *
 * Description:	Initialize a less-than expression.
 */

LessThan::LessThan(Expression *left, Expression *right, const Type &type)
    : Binary(left, right, type)
{
}


/*
 * Function:	GreaterThan
 *
 * Description:	Initialize a greater-than expression.
 */

GreaterThan::GreaterThan(Expression *left, Expression *right, const Type &type)
    : Binary(left, right, type)
{
}


/*
 * Function:	LessOrEqual
 *
 * Description:	Initialize a less-than-or-equal expression.
 */

LessOrEqual::LessOrEqual(Expression *left, Expression *right, const Type &type)
    : Binary(left, right, type)
{
}


/*
 * Function:	GreaterOrEqual
 *
 * Description:	Initialize a greater-than-or-equal expression.
 */

GreaterOrEqual::GreaterOrEqual(Expression *left, Expression *right, const Type &type)
    : Binary(left, right, type)
{
}


/*
 * Function:	Equal::Equal (constructor)
 *
 * Description:	Initialize an equality expression.
 */

Equal::Equal(Expression *left, Expression *right, const Type &type)
    : Binary(left, right, type)
{
}


/*
 * Function:	NotEqual::NotEqual (constructor)
 *
 * Description:	Initialize an inequality expression.
 */

NotEqual::NotEqual(Expression *left, Expression *right, const Type &type)
    : Binary(left, right, type)
{
}


/*
 * Function:	LogicalAnd::LogicalAnd (constructor)
 *
 * Description:	Initialize a logical-and expression.
 */

LogicalAnd::LogicalAnd(Expression *left, Expression *right, const Type &type)
    : Binary(left, right, type)
{
}


/*
 * Function:	LogicalOr::LogicalOr (constructor)
 *
 * Description:	Initialize a logical-or expression.
 */

LogicalOr::LogicalOr(Expression *left, Expression *right, const Type &type)
    : Binary(left, right, type)
{
}


/*
 * Function:	Assignment::Assignment (constructor)
 *
 * Description:	Initialize an assignment statement.
 */

Assignment::Assignment(Expression *left, Expression *right)
    : _left(left), _right(right)
{
}


/*
 * Function:	Break::Break (constructor)
 *
 * Description:	Initialize a break statement.  (Yes, it does nothing.)
 */

Break::Break()
{
}


/*
 * Function:	Return::Return (constructor)
 *
 * Description:	Initialize a return statement.
 */

Return::Return(Expression *expr)
    : _expr(expr)
{
}


/*
 * Function:	Block::Block (constructor)
 *
 * Description:	Initialize a block statement.
 */

Block::Block(Scope *decls, const Statements &stmts)
    : _decls(decls), _stmts(stmts)
{
}


/*
 * Function:	Block::declarations (accessor)
 *
 * Description:	Return the declarations of this block.
 */

Scope *Block::declarations() const
{
    return _decls;
}


/*
 * Function:	While::While (constructor)
 *
 * Description:	Initialize a while statement.
 */

While::While(Expression *expr, Statement *stmt)
    : _expr(expr), _stmt(stmt)
{
}


/*
 * Function:	For::For (constructor)
 *
 * Description:	Initialize a for statement.
 */

For::For(Statement *init, Expression *expr, Statement *incr, Statement *stmt)
    : _init(init), _expr(expr), _incr(incr), _stmt(stmt)
{
}


/*
 * Function:	If::If (constructor)
 *
 * Description:	Initialize an if-then or if-then-else statement.
 */

If::If(Expression *expr, Statement *thenStmt, Statement *elseStmt)
    : _expr(expr), _thenStmt(thenStmt), _elseStmt(elseStmt)
{
}


/*
 * Function:	Function::Function (constructor)
 *
 * Description:	Initialize a function object.
 */

Function::Function(const Symbol *id, Block *body)
    : _id(id), _body(body)
{
}
