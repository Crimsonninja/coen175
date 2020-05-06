#include "Tree.h"
#include <iostream>

using namespace std;

int caller_size = 0;

static ostream &operator <<(ostream &ostr, Expression *expr) {
  expr->operand(ostr);
  return ostr;
}

void Function::generate() {
  caller_size = 0;
  string func_name = _id->name();
  unsigned parameter_size = _id->type().parameters()->types.size();
  Symbols some_symbols = _body->declarations()->symbols();


  int offset = 8;
  int total_bytes_alloc = 0;

  for (unsigned i = 0;i < parameter_size; ++i) {
    some_symbols[i]->offset = offset;
    // cout << "Symbol: " << some_symbols[i]->name() << "\t offset: " << offset << endl;
    offset = offset + some_symbols[i]->type().size();
    while (offset%4!=0)
      offset++;
  }

  // total_bytes_alloc = offset;

  offset = 0;

  for (unsigned i = parameter_size; i < some_symbols.size(); ++i) {
    offset = offset - some_symbols[i]->type().size();
    some_symbols[i]->offset = offset;
    // cout << "Symbol: " << some_symbols[i]->name() << "\t offset: " << offset << endl;
    while (offset%4!=0)
      offset--;
  }

  total_bytes_alloc = total_bytes_alloc - offset;

  while(total_bytes_alloc%16!=8)
    ++total_bytes_alloc;

  // prologue
  cout << ".globl " << func_name << endl;
  cout << func_name << ": pushl\t%ebp" << endl;
  cout << "movl %esp,\t%ebp" << endl;
  cout << "subl $" << func_name << ".size,\t%esp" << endl;

  _body->generate();

  //epilogue
  cout << "movl %ebp,\t%esp" << endl;
  cout << "popl %ebp" << endl;
  cout << "ret" << endl;
  cout << ".set " << func_name << ".size, " << total_bytes_alloc+caller_size << endl;
}

void Block::generate() {
  for (auto stmt:_stmts)
    stmt->generate();
}

void Statement::generate() {

}

void Assignment::generate() {
  cout << "movl " << _right << ", " << _left << endl;
}

void Call::generate() {
  int offset = 0;
  for (auto arg:_args) {
    cout << "movl " << arg << ", %eax" << endl;
    cout << "movl " << "%eax, " << offset << "(%esp)" << endl;
    offset = offset + arg->type().size();
    while (offset%4!=0)
      offset++;
  }

  cout << "call " << _id->name() << endl;

  if (caller_size < offset)
    caller_size = offset;

}

void globalVars(Scope* gScope) {
  Symbols symbols = gScope->symbols();
  for (auto symbol:symbols) {
    if (!symbol->type().isFunction() && !symbol->type().isError())
      cout << ".comm " << symbol->name() << ", " << symbol->type().size() << endl;
  }
}

void Identifier::operand(ostream &ostr) {
  int offset = _symbol->offset;
  if (offset==0)
    cout << _symbol->name();
  else {
    cout << offset << "(%ebp)";
  }
}

void Integer::operand(ostream &ostr) {
  ostr << "$" << _value;
}

void Expression::operand(ostream &ostr) {

}
