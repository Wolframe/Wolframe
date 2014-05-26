#include "filter/filterdef.hpp"
#include "utils/parseUtils.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static const utils::CharTable g_optab( "(),=");

static std::vector<langbind::FilterArgument> parseFilterArguments( std::string::const_iterator& si, const std::string::const_iterator& se)
{
	std::vector<langbind::FilterArgument> rt;
	char ch;
	std::string tok;
	std::string val;

	for (;;)
	{
		// parse argument identifier or value:
		ch = utils::parseNextToken( tok, si, se, g_optab);
		if (!ch) throw std::runtime_error("brackets not balanced in filter arguments");
		if (ch == ')')
		{
			return rt;
		}
		else if (ch == '(' || ch == ',' || ch == '=')
		{
			throw std::runtime_error( std::string( "syntax error in filter arguments: token expected instead of comma (") + (char)ch + ")");
		}

		// check for assignment:
		ch = utils::parseNextToken( val, si, se, g_optab);
		if (ch == ',')
		{
			rt.push_back( langbind::FilterArgument( "", tok));
			continue;
		}
		else if (ch == ')')
		{
			rt.push_back( langbind::FilterArgument( "", tok));
			return rt;
		}
		else if (ch == '=')
		{
			// for assignment get value:
			ch = utils::parseNextToken( val, si, se, g_optab);
			if (ch == ')' || ch == '(' || ch == ',' || ch == '=')
			{
				throw std::runtime_error( "syntax error in filter arguments: unexpected operator");
			}
			rt.push_back( langbind::FilterArgument( tok, val));

			ch = utils::parseNextToken( val, si, se, g_optab);
			if (ch == ',')
			{
				continue;
			}
			else if (ch == ')')
			{
				return rt;
			}
		}
	}
}

FilterDef FilterDef::parse( std::string::const_iterator& si, const std::string::const_iterator& se, const proc::ProcessorProviderInterface* provider)
{
	FilterDef rt;
	std::string filtername;
	char ch = utils::parseNextToken( filtername, si, se);
	if (ch == '"' || ch == '\'') throw std::runtime_error("identifier (filter name) expected");
	ch = utils::gotoNextToken( si, se);
	if (ch == '(')
	{
		++si;
		rt.arg = parseFilterArguments( si, se);
	}
	rt.filtertype = provider->filterType( filtername);
	if (!rt.filtertype)
	{
		throw std::runtime_error(std::string("undefined filter in filter definition '") + filtername + "'");
	}
	return rt;
}

