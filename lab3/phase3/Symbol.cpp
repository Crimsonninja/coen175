#include "Symbol.h"

using namespace std;

Symbol::Symbol()
{

}

Symbol::Symbol(const string& name, const Type& type)
              :_name(name), _type(type)
{

}
