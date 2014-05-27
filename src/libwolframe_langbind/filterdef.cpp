/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
All rights reserved.

This file is part of Project Wolframe.

Commercial Usage
Licensees holding valid Project Wolframe Commercial licenses may
use this file in accordance with the Project Wolframe
Commercial License Agreement provided with the Software or,
alternatively, in accordance with the terms contained
in a written agreement between the licensee and Project Wolframe.

GNU General Public License Usage
Alternatively, you can redistribute this file and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolframe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
/// \file filterdef.cpp
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

