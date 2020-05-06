/*
 * File:	generator.cpp
 *
 * Description:	This file contains the public and member function
 *		definitions for the code generator for Simple C.
 *
 *		Extra functionality:
 *		- putting all the global declarations at the end
 */

# include <cassert>
# include <iostream>
# include "generator.h"
# include "machine.h"
# include "Tree.h"
# include "string.h"
# include <unordered_map>

# define FP(expr) ((expr)->type().isReal())
# define BYTE(expr) ((expr)->type().size() == 1)

using namespace std;

static int offset;
static unsigned max_args;

static Label globalReturn;

static unordered_map<string, Label> m1;
static unordered_map<string, Label> m2;

static vector<Label> breaks;

/*
 * Function:	align (private)
 *
 * Description:	Return the number of bytes necessary to align the given
 *		offset on the stack.
 */

static int align(int offset) {
  if (offset % STACK_ALIGNMENT == 0)
    return 0;

  return STACK_ALIGNMENT - (abs(offset) % STACK_ALIGNMENT);
}


/*
 * Function:	operator << (private)
 *
 * Description:	Convenience function for writing the operand of an
 *		expression using the output stream operator.
 */

static ostream &operator <<(ostream &ostr, Expression *expr) {
  expr->operand(ostr);
  return ostr;
}


/*
 * Function:	Expression::operand
 *
 * Description:	Write an expression as an operator to the specified stream.
 */

void Expression::operand(ostream &ostr) const {
  assert(offset != 0);
  ostr << offset << "(%ebp)";
}


/*
 * Function:	Identifier::operand
 *
 * Description:	Write an identifier as an operand to the specified stream.
 */

void Identifier::operand(ostream &ostr) const {
  if (_symbol->offset == 0)
    ostr << global_prefix << _symbol->name();
  else
    ostr << _symbol->offset << "(%ebp)";
}


/*
 * Function:	Integer::operand
 *
 * Description:	Write an integer as an operand to the specified stream.
 */

void Integer::operand(ostream &ostr) const {
  ostr << "$" << _value;
}

void String::operand(ostream &ostr) const {
  Label my_string;
  unordered_map<string,Label>::const_iterator found = m1.find(_value);
  // Label found = m1.find(_value);
  if (found!=m1.end()) {  // found
    my_string = found->second;
  }
  else {
    // my_string = Label();
    m1.insert({_value, my_string});
  }

  ostr << ".L" <<my_string.number();
  // ostr << leal << my_string << ", %eax" << endl;
}

void Real::operand(ostream &ostr) const {
  Label my_string;
  unordered_map<string,Label>::const_iterator found = m2.find(_value);
  // Label found = m1.find(_value);
  if (found!=m2.end()) {  // found
    my_string = found->second;
  }
  else {
    // my_string = Label();
    m2.insert({_value, my_string});
  }

  ostr << ".L" <<my_string.number();
  // ostr << leal << my_string << ", %eax" << endl;
}

void assigntemp(Expression *expr) {
  offset -= expr->type().size();
  expr->offset = offset;
}

/*
 * Function:	Call::generate
 *
 * Description:	Generate code for a function call expression.
 */

void Call::generate() {
  unsigned offset, size;

  /* Generate code for all arguments first. */

  for (auto arg : _args)
    arg->generate();

  /* Move the arguments onto the stack. */

  offset = 0;

  for (auto arg : _args) {
    if (FP(arg)) {
	    cout << "\tfldl\t" << arg << endl;
	    cout << "\tfstpl\t" << offset << "(%esp)" << endl;
    }
    else {
      cout << "\tmovl\t" << arg << ", %eax" << endl;
      cout << "\tmovl\t%eax, " << offset << "(%esp)" << endl;
    }

    size = arg->type().size();
    offset += size;
  }

  if (offset > max_args)
    max_args = offset;

  /* Make the function call. */

  cout << "\tcall\t" << global_prefix << _id->name() << endl;

  /* Save the return value */

//   # if 0
    assigntemp(this);

  if (FP(this))
    cout << "\tfstpl\t" << this << endl;
  else
    cout << "\tmovl\t%eax, " << this << endl;
// # endif
}


/*
 * Function:	Block::generate
 *
 * Description:	Generate code for this block, which simply means we
 *		generate code for each statement within the block.
 */

void Block::generate() {
  for (auto stmt : _stmts) {
    stmt->generate();
  }
}


/*
 * Function:	Function::generate
 */

void Function::generate() {
  Label empty;
  globalReturn = empty;
  max_args = 0;
  offset = SIZEOF_REG * 2;
  allocate(offset);

  /* Generate our prologue. */
  cout << "#Prologue" << endl;
  cout << global_prefix << _id->name() << ":" << endl;
  cout << "\tpushl\t%ebp" << endl;
  cout << "\tmovl\t%esp, %ebp" << endl;
  cout << "\tsubl\t$" << _id->name() << ".size, %esp" << endl;

  /* Generate the body of this function. */

  _body->generate();

  /* Compute the proper stack frame size. */

  offset -= max_args;
  offset -= align(offset - SIZEOF_REG * 2);


  /* Generate our epilogue. */

  cout << globalReturn << ": " << endl;
  cout << "#Epilogue" << endl;
  cout << "\tmovl\t%ebp, %esp" << endl;
  cout << "\tpopl\t%ebp" << endl;
  cout << "\tret" << endl << endl;

  cout << "\t.set\t" << _id->name() << ".size, " << -offset << endl;
  cout << "\t.globl\t" << global_prefix << _id->name() << endl << endl;
}


/*
 * Function:	generateGlobals
 *
 * Description:	Generate code for any global variable declarations.
 */

void generateGlobals(Scope *scope) {
  const Symbols &symbols = scope->symbols();

  for (auto symbol : symbols)
    if (!symbol->type().isFunction()) {
      cout << "\t.comm\t" << global_prefix << symbol->name() << ", ";
      cout << symbol->type().size() << endl;
	}

  cout << ".data" << endl;

  for (pair<std::string, Label> element : m1) {
    cout << ".L" << element.second.number() << ":\t.asciz\t" << "\"" << escapeString(element.first) << "\"" << endl;
  }

  for (pair<std::string, Label> element : m2) {
    cout << ".L" << element.second.number() << ":\t.double\t"  << escapeString(element.first) << endl;
  }
}


/*
 * Function:	Assignment::generate
 *
 * Description:	Generate code for an assignment statement.
 *
 *		NOT FINISHED: Only works if the right-hand side is an
 *		integer literal and the left-hand side is an integer
 *		scalar.
 */

void Assignment::generate() {
  cout << "#Assigning" << endl;
  _right->generate();
  cout << "#Generated right" << endl;
  // assigntemp(this);
  Expression * leftChild = _left->isDereference();

  if (leftChild!=nullptr) {
    cout << "#Generating left child" << endl;
    leftChild->generate();

    if (FP(_right)) {   // floating
      cout << "\tfldl\t" << _right << endl;
      cout << "\tmovl\t" << leftChild << ", %eax" << endl;
      cout << "\tfstpl\t" << "(%eax)" << endl;
    }
    else if (BYTE(_right)) {  // char
      cout << "\tmovl\t" << _right << ", %eax" << endl;
      cout << "\tmovl\t" << leftChild << ", %ecx" << endl;
      cout << "\tmovb\t" << "%eax" << ", (%ecx)" << endl;
    }
    else {    // int or pointer
      cout << "#INT Pointer assign" << endl;
      cout << "\tmovl\t" << _right << ", %eax" << endl;
      cout << "\tmovl\t" << leftChild << ", %ecx" << endl;
      cout << "\tmovl\t" << "%eax" << ", (%ecx)" << endl;
    }
  }
  else {
    cout << "#No dereference here" << endl;
    _left->generate();
    if (FP(_right)) {   // floating
      cout << "\tfldl\t" << _right << endl;
      cout << "\tfstpl\t" << _left << endl;
    }
    else if (BYTE(_right)) {  // char
      cout << "\tmovb\t" << _right << ", %al" << endl;
      cout << "\tmovb\t" << "%al, " << _left << endl;
    }
    else {    // int or pointer
      cout << "\tmovl\t" << _right << ", %eax" << endl;
      cout << "\tmovl\t" << "%eax, " << _left << endl;
    }
  }
}

Expression * Expression::isDereference() {
  return nullptr;
}

Expression * Dereference::isDereference() {
  return _expr;
}

void Multiply::generate() {
  cout << "#Multiplying" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);

  if (FP(this)) {
    cout << "\tfldl\t" << _left << endl;
    cout << "\tfmull\t" << _right << endl;
    cout << "\tfstpl\t" << this << endl;
  }
  else {
    cout << "\tmovl\t" << _left << ", %eax" << endl;
    cout << "\timull\t" << _right << ", %eax" << endl;
    cout << "\tmovl\t%eax, " << this << endl;
  }
}

void Divide::generate() {
  cout << "#Dividing" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);

  if(FP(this)) {
    cout << "\tfldl\t" << _left << endl;
    cout << "\tfdivl\t" << _right << endl;
    cout << "\tfstpl\t" << this << endl;
  }
  else {
    cout << "\tmovl\t" << _left << ", %eax"<< endl;     // load: %eax allocated
    cout << "\tcltd\t" << endl;    // sign extend %eax into %edx
    cout << "\tmovl\t" << _right << ", %ecx" << endl;
    cout << "\tidivl\t" << "%ecx" << endl;               // %edx:%eax / y
    cout << "\tmovl\t%eax, " << this << endl;
  }
}

void Remainder::generate() {
  cout << "#Remainding" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);

  // Floating Point has no remainder
  cout << "\tmovl\t" << _left << ", %eax"<< endl;     // load: %eax allocated
  cout << "\tcltd\t" << endl;    // sign extend %eax into %edx
  cout << "\tmovl\t" << _right << ", %ecx" << endl;
  cout << "\tidivl\t" << "%ecx" << endl;               // %edx:%eax / y
  cout << "\tmovl\t%edx, " << this << endl;
}

void Add::generate() {
  cout << "#Adding" << endl;
  _left->generate();
  _right->generate();
  assigntemp(this);

  if(FP(this)) {
    cout << "\tfldl\t" << _left << endl;
    cout << "\tfaddl\t" << _right << endl;
    cout << "\tfstpl\t" << this << endl;
  }
  else {
    cout << "\tmovl\t" << _left << ", %eax"<< endl;
    if (scaleLeft!=0)
      cout << "\timull\t$" << scaleLeft << ", %eax" << endl;

    cout << "\tmovl\t" << _right << ", %ecx"<< endl;
    if (scaleRight!=0)
        cout << "\timull\t$" << scaleRight << ", %ecx" << endl;
    cout << "\taddl\t" << "%ecx" << ", %eax"<< endl;
    cout << "\tmovl\t%eax, " << this << endl;
  }
}

void Subtract::generate() {
    cout << "#Subtracting" << endl;
    _left->generate();
    _right->generate();
    assigntemp(this);

    if(FP(this)) {
        cout << "\tfldl\t" << _left << endl;
        cout << "\tfsubl\t" << _right << endl;
        cout << "\tfstpl\t" << this << endl;
    }
    else {
        cout << "\tmovl\t" << _left << ", %eax" << endl;
        if (scaleResult!=0) {
          cout << "\tsubl\t" << _right << ", %eax"<< endl;
          cout << "\tidivl\t$" << scaleResult << endl;
        }
        else if (scaleRight!=0) {
          cout << "\timull\t$" << scaleRight << ", %ecx" << endl;
          cout << "\tsubl\t" << "%ecx" << ", %eax" << endl;
        }
        else {
          cout << "\tsubl\t" << _right << ", %eax" << endl;
        }
        cout << "\tmovl\t%eax, " << this << endl;
    }
}

void Not::generate() {
  _expr->generate();
  assigntemp(this);

  if (FP(this)) {
    cout << "\tfldl\t" << _expr << endl;
    cout << "\tftst\t" << endl;
    cout << "\tfnstsw\t" << "%ax" << endl;
    cout << "\tfstp\t" << "%st(0)" << endl;
    cout << "\tsahf\t" << endl;
    cout << "\tsete\t" << "%al" << endl;
    cout << "\tmovzbl\t" << "%al" << ", %eax" << endl;
    cout << "\tmovl\t%eax, " << this << endl;
  }
  else {
    cout << "\tmovl\t" << _expr << ", %eax" << endl;
    cout << "\tcmpl\t" << "$0" << ", %eax" << endl;
    cout << "\tsete\t" << "%al" << endl;
    cout << "\tmovzbl\t" << "%al" << ", %eax" << endl;
    cout << "\tmovl\t%eax, " << this << endl;
  }
}

void Negate::generate() {
  _expr->generate();
  assigntemp(this);

  if (FP(this)) {
    cout << "\tfldl\t" << _expr << endl;
    cout << "\tfchs\t" << endl;
    cout << "\tfstpl\t" << this << endl;
  }
  else {
    cout << "\tmovl\t" << _expr << ", %eax" << endl;
    cout << "\tnegl\t" << "%eax" << endl;
    cout << "\tmovl\t%eax, " << this << endl;
  }
}

void Dereference::generate() {
    _expr->generate();
    assigntemp(this);
    cout << "#Dereference" << endl;
    cout << "\tmovl\t" << _expr << ", %eax" << endl;

    if (FP(this)) {
        cout << "\tfldl\t" << "(%eax)" << endl;
        cout << "\tfstpl\t" << this << endl;
    }
    else {
        cout << "\tmovl\t" << "(%eax)" << ", %eax" << endl;
        cout << "\tmovl\t%eax, " << this << endl;
    }
}

void Address::generate() {
    _expr->generate();
    assigntemp(this);
    cout << "#Addressing" << endl;

    if (_expr->isDereference()==nullptr)
        cout << "\tleal\t" << _expr << ", %eax" << endl;
    else
        cout << "\tmovl\t" << _expr->isDereference() << ", %eax"<<endl;
    cout << "\tmovl\t%eax, " << this << endl;
}

void LessThan::generate() {
    _left->generate();
    _right->generate();
    assigntemp(this);

    if (FP(this)) {
        cout << "\tfldl\t" << _left << endl;
        cout << "\tfcompl\t" << _right << endl;
        cout << "\tfnstsw\t" << "%ax" << endl;
        cout << "\tsahf\t" << endl;
        cout << "\tsetb\t" << "%al" << endl;
        cout << "\tmovzbl\t" << "%al" << ", %eax" << endl;
        cout << "\tmovl\t%eax, " << this << endl;
    }
    else {
        cout << "\tmovl\t" << _left << ", %eax" << endl;
        cout << "\tcmpl\t" << _right << ", %eax" << endl;
        cout << "\tsetl\t" << "%al" << endl;
        cout << "\tmovzbl\t" << "%al" << ", %eax" << endl;
        cout << "\tmovl\t%eax, " << this << endl;
    }
}

void GreaterThan::generate() {
    _left->generate();
    _right->generate();
    assigntemp(this);

    if (FP(this)) {
        cout << "\tfldl\t" << _left << endl;
        cout << "\tfcompl\t" << _right << endl;
        cout << "\tfnstsw\t" << "%ax" << endl;
        cout << "\tsahf\t" << endl;
        cout << "\tseta\t" << "%al" << endl;
        cout << "\tmovzbl\t" << "%al" << ", %eax" << endl;
        cout << "\tmovl\t%eax, " << this << endl;
    }
    else {
        cout << "\tmovl\t" << _left << ", %eax" << endl;
        cout << "\tcmpl\t" << _right << ", %eax" << endl;
        cout << "\tsetg\t" << "%al" << endl;
        cout << "\tmovzbl\t" << "%al" << ", %eax" << endl;
        cout << "\tmovl\t%eax, " << this << endl;
    }
}

void LessOrEqual::generate() {
    _left->generate();
    _right->generate();
    assigntemp(this);

    if (FP(this)) {
        cout << "\tfldl\t" << _left << endl;
        cout << "\tfcompl\t" << _right << endl;
        cout << "\tfnstsw\t" << "%ax" << endl;
        cout << "\tsahf\t" << endl;
        cout << "\tsetbe\t" << "%al" << endl;
        cout << "\tmovzbl\t" << "%al" << ", %eax" << endl;
        cout << "\tmovl\t%eax, " << this << endl;
    }
    else {
        cout << "\tmovl\t" << _left << ", %eax" << endl;
        cout << "\tcmpl\t" << _right << ", %eax" << endl;
        cout << "\tsetle\t" << "%al" << endl;
        cout << "\tmovzbl\t" << "%al" << ", %eax" << endl;
        cout << "\tmovl\t%eax, " << this << endl;
    }
}

void GreaterOrEqual::generate() {
    _left->generate();
    _right->generate();
    assigntemp(this);

    if (FP(this)) {
        cout << "\tfldl\t" << _left << endl;
        cout << "\tfcompl\t" << _right << endl;
        cout << "\tfnstsw\t" << "%ax" << endl;
        cout << "\tsahf\t" << endl;
        cout << "\tsetae\t" << "%al" << endl;
        cout << "\tmovzbl\t" << "%al" << ", %eax" << endl;
        cout << "\tmovl\t%eax, " << this << endl;
    }
    else {
        cout << "\tmovl\t" << _left << ", %eax" << endl;
        cout << "\tcmpl\t" << _right << ", %eax" << endl;
        cout << "\tsetge\t" << "%al" << endl;
        cout << "\tmovzbl\t" << "%al" << ", %eax" << endl;
        cout << "\tmovl\t%eax, " << this << endl;
    }
}

void Equal::generate() {
    _left->generate();
    _right->generate();
    assigntemp(this);

    if (FP(_left)) {
        cout << "\tfldl\t" << _left << endl;
        cout << "\tfcompl\t" << _right << endl;
        cout << "\tfnstsw\t" << "%ax" << endl;
        cout << "\tsahf\t" << endl;
        cout << "\tsete\t" << "%al" << endl;
        cout << "\tmovzbl\t" << "%al" << ", %eax" << endl;
        cout << "\tmovl\t%eax, " << this << endl;
    }
    else {
        cout << "#Equality!" << endl;
        cout << "\tmovl\t" << _left << ", %eax" << endl;
        cout << "\tcmpl\t" << _right << ", %eax" << endl;
        cout << "\tsete\t" << "%al" << endl;
        cout << "\tmovzbl\t" << "%al" << ", %eax" << endl;
        cout << "\tmovl\t%eax, " << this << endl;
    }
}

void NotEqual::generate() {
    _left->generate();
    _right->generate();
    assigntemp(this);

    if (FP(this)) {
        cout << "\tfldl\t" << _left << endl;
        cout << "\tfcompl\t" << _right << endl;
        cout << "\tfnstsw\t" << "%ax" << endl;
        cout << "\tsahf\t" << endl;
        cout << "\tsetne\t" << "%al" << endl;
        cout << "\tmovzbl\t" << "%al" << ", %eax" << endl;
        cout << "\tmovl\t%eax, " << this << endl;
    }
    else {
        cout << "\tmovl\t" << _left << ", %eax" << endl;
        cout << "\tcmpl\t" << _right << ", %eax" << endl;
        cout << "\tsetne\t" << "%al" << endl;
        cout << "\tmovzbl\t" << "%al" << ", %eax" << endl;
        cout << "\tmovl\t%eax, " << this << endl;
    }
}

void LogicalOr::generate() {
  _left->generate();
  _right->generate();
  assigntemp(this);

  Label firstLabel, secondLabel;
  cout << "#LogicalOrring" << endl;
  if (FP(this)) {
    cout << "\tfldl\t" << _left << endl;
    cout << "\tftst\t" << endl;
    cout << "\tfnstsw\t" << "%ax" << endl;
    cout << "\tfstp\t" << "%st(0)" << endl;
    cout << "\tsahf" << endl;
    cout << "\tjne\t" << firstLabel << endl;

    cout << "\tmovl\t" << "$0" << ", %eax" << endl;

    cout << "\tfldl\t" << _right << endl;
    cout << "\tftst\t" << endl;
    cout << "\tfnstsw\t" << "%ax" << endl;
    cout << "\tfstp\t" << "%st(0)" << endl;
    cout << "\tsahf" << endl;
    cout << "\tjne\t" << secondLabel << endl;
    cout << firstLabel << ":" << endl;
    cout << "\tmovl\t" << "$1" << ", %eax" << endl;
    cout << secondLabel << ":" << endl;
    cout << "\tmovl\t%eax, " << this << endl;
  }
  else {
    cout << "\tmovl\t" << _left << ", %eax" << endl;
    cout << "\tcmpl\t" << "$0" << ", %eax" << endl;
    cout << "\tjne\t" << firstLabel << endl;
    cout << "\tmovl\t" << _right << ", %eax" << endl;
    cout << "\tcmpl\t" << "$0" << ", %eax" << endl;
    cout << "\tjne\t" << firstLabel << endl;
    cout << "\tmovl\t" << "$0" << ", %eax" << endl;
    cout << "\tjmp\t" << secondLabel << endl;
    cout << firstLabel << ":" << endl;
    cout << "\tmovl\t" << "$1" << ", %eax" << endl;
    cout << secondLabel << ":" << endl;
    cout << "\tmovl\t%eax, " << this << endl;
  }
}

void LogicalAnd::generate() {
  _left->generate();
  _right->generate();
  assigntemp(this);

  Label firstLabel, secondLabel;

  if (FP(this)) {
    //cout << "\tfldl\t" << _left << endl;
  }
  else {
    cout << "#LogicalAnding" << endl;
    cout << "\tmovl\t" << _left << ", %eax" << endl;
    cout << "\tcmpl\t" << "$0" << ", %eax" << endl;
    cout << "\tje\t" << firstLabel << endl;
    cout << "\tmovl\t" << _right << ", %eax" << endl;
    cout << "\tcmpl\t" << "$0" << ", %eax" << endl;
    cout << "\tje\t" << firstLabel << endl;
    cout << "\tmovl\t" << "$1" << ", %eax" << endl;
    cout << "\tjmp\t" << secondLabel << endl;
    cout << firstLabel << ":" << endl;
    // cout << "#Got here" << endl;
    cout << "\tmovl\t" << "$0" << ", %eax" << endl;
    cout << secondLabel << ":" << endl;
    cout << "\tmovl\t%eax, " << this << endl;
    // cout << "#Second Label" << endl;
    // assigntemp(this);
  }
}

void Increment::generate() {
  _expr->generate();
  assigntemp(this);

  Expression * child = _expr->isDereference();
  if (child!=nullptr) {
    cout << "#Deref increment" << endl;
    if (FP(this)) {
      cout << "#Deref increment for FP" << endl;
      cout << "\tfldl\t" << _expr << endl;
      cout << "\tfld1\t" << endl;
      cout << "\tfaddp\t" << endl;
      cout << "\tmovl\t" << child << ", %ecx" << endl;
      cout << "\tfstpl\t" << "(%ecx)" << endl;
    }
    else {
      cout << "#Deref increment for non FP" << endl;
      cout << "\tmovl\t" << _expr << ", %eax" << endl;
      cout << "\taddl\t$" << scale << ", %eax"<< endl;
      cout << "\tmovl\t" << child << ", %ecx" << endl;
      cout << "\tmovl\t" << "%eax" << ", (%ecx)" << endl;
    }
  }
  else {
    cout << "#Nonderef increment" << endl;
    if (FP(this)) {
      cout << "#Nonderef increment for FP" << endl;
      cout << "\tfldl\t" << _expr << endl;
      cout << "\tfld1\t" << endl;
      cout << "\tfaddp\t" << endl;
      cout << "\tfstpl\t" << this << endl;
      //Must make sure that new calculation is also stored into expr
      cout << "\tfldl\t" << this << endl;
      cout << "\tfstpl\t" << _expr << endl;
    }
    else {
      cout << "#Nonderef increment for non FP" << endl;
      cout << "\tmovl\t" << _expr << ", %eax" << endl;
      cout << "\taddl\t" << "$1" << ", %eax"<< endl;
      cout << "\tmovl\t%eax, " << this << endl;
      //Must make sure that new calculation is also stored into expr
      cout << "\tmovl\t%eax, " << _expr << endl;
    }
  }
}

void Decrement::generate() {
  _expr->generate();
  assigntemp(this);

  Expression * child = _expr->isDereference();
  if (child!=nullptr) {
    cout << "#Deref increment" << endl;
    if (FP(this)) {
      cout << "#Deref increment for FP" << endl;
      cout << "\tfld1\t" << endl;
      cout << "\tfldl\t" << _expr << endl;
      cout << "\tfsubp\t" << endl;
      cout << "\tmovl\t" << child << ", %ecx" << endl;
      cout << "\tfstpl\t" << "(%ecx)" << endl;
    }
    else {
      cout << "#Deref increment for non FP" << endl;
      cout << "\tmovl\t" << _expr << ", %eax" << endl;
      cout << "\tsubl\t$" << scale << ", %eax"<< endl;
      cout << "\tmovl\t" << child << ", %ecx" << endl;
      cout << "\tmovl\t" << "%eax" << ", (%ecx)" << endl;
    }
  }
  else {
    cout << "#Nonderef increment" << endl;
    if (FP(this)) {
      cout << "#Nonderef increment for FP" << endl;
      cout << "\tfld1\t" << endl;
      cout << "\tfldl\t" << _expr << endl;
      cout << "\tfsubp\t" << endl;
      cout << "\tfstpl\t" << this << endl;
      //Must make sure that new calculation is also stored into expr
      cout << "\tfldl\t" << this << endl;
      cout << "\tfstpl\t" << _expr << endl;
    }
    else {
      cout << "#Nonderef increment for non FP" << endl;
      cout << "\tmovl\t" << _expr << ", %eax" << endl;
      cout << "\tsubl\t" << "$1" << ", %eax"<< endl;
      cout << "\tmovl\t%eax, " << this << endl;
      //Must make sure that new calculation is also stored into expr
      cout << "\tmovl\t%eax, " << _expr << endl;
    }
  }
}

void Cast::generate() {
  _expr->generate();
  assigntemp(this);
  cout << "#Casting" << endl;
  if (this->type().isNumeric()&&_expr->type().isNumeric()) {
    if (FP(this)) {
      if (FP(_expr)) {    // double to double
        cout << "\tfldl\t" << _expr << endl;
        cout << "\tfstpl\t" << this << endl;
      }
      else {
        if BYTE(_expr) { // char to double
          cout << "\tfildl\t" << _expr << endl; // this may be different
          cout << "\tfstpl\t" << this << endl;
        }
        else {    // int to double
          cout << "\tfildl\t" << _expr << endl;
          cout << "\tfstpl\t" << this << endl;
        }
      }
    }
    else if (BYTE(this)){
      if (FP(_expr)) {    // double to char
        cout << "\tfldl\t" << _expr << endl;
        cout << "\tfisttpl\t" << this << endl;
        cout << "\tmovl\t" << this << ", %eax"<< endl;
        cout << "\tmovb\t" << "%al, " << this << endl;
      }
      else {
        if BYTE(_expr) { // char to char
          cout << "\tmovb\t" << _expr << ", %eax" << endl;
          cout << "\tmovl\t%eax, " << this << endl;
        }
        else {    // int to char
          cout << "\tmovl\t" << _expr << ", %eax" << endl;
          cout << "\tmovb\t%eax, " << this << endl;
        }
      }
    }
    else {
      if (FP(_expr)) {    // double to int
        cout << "\tfldl\t" << _expr << endl;
        cout << "\tfisttpl\t" << this << endl;
      }
      else {
        if BYTE(_expr) { // char to int
          cout << "\tmovsbl\t" << _expr << ", %eax" << endl;
          cout << "\tmovl\t%eax, " << this << endl;
        }
        else {    // int to int
          cout << "\tmovl\t" << _expr << ", %eax" << endl;
          cout << "\tmovl\t%eax, " << this << endl;
        }
      }
    }
  }
  else {
    cout << "\tmovl\t" << _expr << ", %eax" << endl;
    cout << "\tmovl\t%eax, " << this << endl;
  }
}

void Expression::test(const Label &label, bool ifTrue) {
  generate();
  if (FP(this)) {
    cout << "\tfldl\t" << this << endl;
    cout << "\tftst\t" << endl;
    cout << "\tfnstsw\t" << "%ax" << endl;
    cout << "\tsahf" << endl;
  }
  else {
    cout << "\tmovl\t" << this << ", %eax" << endl;
    cout << "\tcmpl\t$0, %eax" << endl;
  }

  cout << (ifTrue ? "\tjne\t" : "\tje\t") << label << endl;
}

void While::generate() {
  Label loop, exit;
  breaks.push_back(exit);
  cout << loop << ":" << endl;

  _expr->test(exit, false);
  _stmt->generate();

  cout << "\tjmp\t" << loop << endl;
  cout << exit << ":" << endl;
}

void For::generate() {
  Label loop, exit;
  breaks.push_back(exit);
  _init->generate();
  cout << loop << ":" << endl;

  _expr->test(exit, false);
  _stmt->generate();
  _incr->generate();
  cout << "\tjmp\t" << loop << endl;
  cout << exit << ":" << endl;
}

void If::generate() {
  Label skip;

  cout << "#If" << endl;
  _expr->test(skip, false);
  _thenStmt->generate();

  if (!_elseStmt) {
    cout << skip << ":" << endl;
  } else {
    Label exit;
    cout << "\tjmp\t" << exit << endl;
    cout << skip << ":" << endl;
    _elseStmt->generate();
    cout << exit << ":" << endl;
  }
}

void Return::generate() {
  _expr->generate();

  if (FP(_expr)) {
    cout << "\tfldl\t" << _expr << endl;
  }
  else {
    cout << "\tmovl\t" << _expr << ", %eax" << endl;
  }
  cout << "\tjmp\t" << globalReturn << endl;
}

void Break::generate() {
  cout << "\tjmp\t" << breaks.back() << endl;
  breaks.pop_back();
}
