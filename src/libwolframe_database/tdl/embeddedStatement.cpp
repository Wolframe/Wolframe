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
///\file tdl/embeddedStatement.cpp
#include "tdl/embeddedStatement.hpp"
#include "tdl/elementReference.hpp"
#include "tdl/parseUtils.hpp"
#include <string>
#include <vector>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::tdl;

EmbeddedStatement EmbeddedStatement::parse( const LanguageDescription* langdescr, std::string::const_iterator& si, std::string::const_iterator se)
{
	EmbeddedStatement rt;
	std::string dbstm = langdescr->parseEmbeddedStatement( si, se);
	std::string::const_iterator start = dbstm.begin(), ci = dbstm.begin(), ce = dbstm.end();
	std::string tok;

	char ch = parseNextToken( langdescr, tok, ci, ce);
	for (; ci != ce && ch; ch = parseNextToken( langdescr, tok, ci, ce))
	{
		if (ch == '$' && ci != ce)
		{
			if (*ci == '(' || isAlphaNumeric(*ci) || *ci == '#')
			{
				
				rt.stmstring.append( start, ci - 1);
				start = ci;

				rt.params.push_back( ElementReference::parseEmbeddedReference( langdescr, ci, ce));
				rt.stmstring.append( langdescr->stm_argument_reference( rt.params.size()));
			}
		}
	}
	rt.stmstring.append( start, ci);
	return rt;
}

