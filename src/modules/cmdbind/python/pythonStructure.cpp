#include "pythonStructure.hpp"
#include <sstream>
#include <Python.h>

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
	if (array())
	{
		Structure::const_iterator si = begin(), se = end();
		out << pformat->openstruct;
		int idx = 0;
		for (; si != se; ++si,++idx)
		{
			if (idx) out << pformat->decldelimiter;
			print_newitem( out, pformat, level);
			Structure elem( si->val);
			elem.print( out, pformat, level+1);
		}
		if (idx>0)
		{
			print_newitem( out, pformat, level);
		}
		out << pformat->closestruct;
	}
	else if (atomic())
	{
		out << pformat->startvalue;
		if (pformat->maxitemsize)
		{
			out << utils::getLogString( value(), pformat->maxitemsize);
		}
		else
		{
			out << value().tostring();
		}
		out << pformat->endvalue;
	}
	else
	{
		Structure::const_iterator si = begin(), se = end();
		out << pformat->openstruct;

		int idx = 0;
		for (; si != se; ++si,++idx)
		{
			if (idx) out << pformat->decldelimiter;
			print_newitem( out, pformat, level);

			out << Object::tostring(si->key) << pformat->assign;

			Structure val( si->val);
			val.print( out, pformat, level+1);
		}
		out << pformat->closestruct;
	}
}

std::string Structure::tostring( const utils::PrintFormat* pformat) const
{
	std::ostringstream buf;
	print( buf, pformat, 0);
	return buf.str();
}

