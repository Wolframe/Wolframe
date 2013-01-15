/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\brief Implementation of programs for a postgres database
///\file PostgreSQLprogram.cpp
#include "PostgreSQLprogram.hpp"
#include "config/programBase.hpp"
#include "utils/miscUtils.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

static const utils::CharTable g_optab( ";:-,.=)(<>[]/&%*|+-#?!$");

void PostgreSQLprogram::load( const std::string& source)
{
	config::PositionalErrorMessageBase ERROR(source);
	config::PositionalErrorMessageBase::Message MSG;
	std::string::const_iterator si = source.begin(), se = source.end();
	char ch;
	std::string tok;
	const char* commentopr = "--";

	while ((ch = utils::gotoNextToken( si, se)) != 0)
	{
		if (ch == commentopr[0])
		{
			std::string::const_iterator ti = si;
			std::size_t ci = 0;
			while (commentopr[ci] && commentopr[ci] == *ti)
			{
				ci++;
				ti++;
			}
			if (!commentopr[ci])
			{
				// skip to end of line
				while (ti != se && *ti != '\n') ++ti;
				si = ti;
				continue;
			}
		}
		ch = utils::parseNextToken( tok, si, se, g_optab);
		if ((ch|32) == 'p' && boost::iequals( tok, "PREPARE"))
		{
			std::string stmname;
			ch = utils::parseNextToken( stmname, si, se, g_optab);
			if (!ch) throw ERROR( si, MSG << "unexpected end of file");
			if (g_optab[ch]) throw ERROR( si, MSG << "unexpected token '" << ch << "'");

			ch = utils::parseNextToken( tok, si, se, g_optab);
			if (!ch) throw ERROR( si, MSG << "unexpected end of file");
			if (g_optab[ch]) throw ERROR( si, MSG << "unexpected token '" << ch << "'");
			if (!boost::iequals( tok, "AS") && !boost::iequals( tok, "FROM"))
			{
				throw ERROR( si, MSG << "unexpected token '" << tok << "'");
			}

			std::string::const_iterator stmstart = si;
			while (utils::parseNextToken( tok, si, se, g_optab) != ';');
			std::string stm( stmstart, si - 1);

			m_statementmap.insert( stmname, stm);
		}
		else if (g_optab[ch])
		{
			throw ERROR( si, MSG << "unexpected token '" << ch << "'");
		}
		else
		{
			throw ERROR( si, MSG << "unexpected token '" << tok << "'");
		}
	}
}


