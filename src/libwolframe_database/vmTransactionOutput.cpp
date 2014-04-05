#include "database/vmTransactionOutput.hpp"
#include "utils/printFormats.hpp"
#include <iostream>
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::db;

void VmTransactionOutput::print( std::ostream& out, const utils::PrintFormat* pformat) const
{
	if (!pformat) print( out, utils::logPrintFormat());
	std::size_t indent = 0;
	std::size_t indentsize = (pformat->indent?std::strlen(pformat->indent):0);
	std::string indentstr;

	vm::Output::const_iterator oi = m_impl.begin(), oe = m_impl.end();
	for (; oi != oe; ++oi)
	{
		switch (oi->op())
		{
			case vm::Output::Element::Open:
				out << pformat->newitem << indentstr << oi->arg().tostring() << pformat->openstruct;
				indent += 1;
				indentstr.append( pformat->indent);
				break;

			case vm::Output::Element::Close:
				if (indent == 0)
				{
					++oi;
					if (oi != oe) throw std::runtime_error("transaction function output tags not balanced");
					return;
				}
				out << pformat->newitem << indentstr << pformat->closestruct;
				indent -= 1;
				indentstr.resize( indentstr.size() - indentsize);
				break;

			case vm::Output::Element::Value:
				out << pformat->newitem << indentstr << pformat->startvalue << oi->arg().tostring() << pformat->endvalue; 
				break;
		}
	}
	if (indent != 0)
	{
		throw std::runtime_error("transaction function output tags not balanced");
	}
}

std::string VmTransactionOutput::tostring( const utils::PrintFormat* pformat) const
{
	std::ostringstream out;
	print( out, pformat);
	return out.str();
}



