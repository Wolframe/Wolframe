#include "mylangInterpreter.hpp"
#include "utils/fileUtils.hpp"
#include "logger-v1.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::mylang;

Interpreter::Interpreter()
{
}

std::vector<std::string> Interpreter::loadProgram( const std::string& name)
{
	std::vector<std::string> rt;
	std::vector<std::string> lns = utils::readSourceFileLines( name);
	std::vector<std::string>::const_iterator li = lns.begin(), le = lns.end();
	for (; li != le; ++li)
	{
		std::string prc = boost::trim_copy( *li);
		if (!prc.empty())
		{
			rt.push_back( prc);
		}
	}
	return rt;
}

InterpreterInstanceR Interpreter::getInstance( const std::string& funcname) const
{
	return InterpreterInstanceR( new InterpreterInstance());
}

