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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Implementation of embedded database statement parsing
///\file tdl/preprocCallStatement.cpp
#include "tdl/preprocCallStatement.hpp"
#include "tdl/parseUtils.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::tdl;

static const utils::CharTable g_function_optab( ",)(");
static const utils::CharTable g_function_idtab( "a..zA..Z0..9_.");

void PreProcCallStatement::clear()
{
	name.clear();
	params.clear();
}

PreProcCallStatement PreProcCallStatement::parse( const LanguageDescription* langdescr, std::string::const_iterator& ci, std::string::const_iterator ce)
{
	PreProcCallStatement rt;
	char ch = gotoNextToken( langdescr, ci, ce);
	if (!ch)
	{
		throw std::runtime_error( "unexpected end of transaction. Preprocessing call expected");
	}
	if (!isAlphaNumeric( ch))
	{
		throw std::runtime_error( "unexpected token (function name expected)");
	}
	(void)utils::parseNextToken( rt.name, ci, ce, g_function_optab, g_function_idtab);

	ch = gotoNextToken( langdescr, ci, ce);
	if (ch != '(')
	{
		throw std::runtime_error( "'(' expected after function name");
	}
	++ci; ch = utils::gotoNextToken( ci, ce);
	if (!ch) throw std::runtime_error( "unexpected end of transaction description. Function parameter list expected");

	// Parse parameter list:
	while (ch != ')')
	{
		rt.params.push_back( PreProcElementReference::parse( langdescr, ci, ce));
		ch = utils::gotoNextToken( ci, ce);
		if (ch == ',')
		{
			++ci;
		}
		else if (ch != ')')
		{
			throw std::runtime_error( "unexpected token (comma or close bracket excepted as separator in parameter list)");
		}
	}
	return rt;
}

