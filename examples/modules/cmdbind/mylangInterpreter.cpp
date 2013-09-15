#include "mylangInterpreter.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::mylang;

Interpreter::Interpreter()
{
}

std::vector<std::string> Interpreter::loadProgram( const std::string& name)
{
	std::vector<std::string> rt;
	// TODO: myLang has sort of entry points which are added here to the list
	// of entry points (functions)?
	return rt;
}

InterpreterInstanceR Interpreter::getInstance( const std::string& funcname) const
{
	return InterpreterInstanceR( 0 );
}

