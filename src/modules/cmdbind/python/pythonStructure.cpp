#include "pythonStructure.hpp"
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::python;

static void print_newitem( std::ostream& out, const utils::PrintFormat* pformat, std::size_t level)
{
	out << pformat->newitem;
	for (std::size_t ll=0; ll<level; ++ll) out << pformat->indent;
}

void Structure::print( std::ostream& out, const utils::PrintFormat* pformat, std::size_t level) const
{
}

std::string Structure::tostring( const utils::PrintFormat* pformat) const
{
	std::ostringstream buf;
	print( buf, pformat, 0);
	return buf.str();
}

