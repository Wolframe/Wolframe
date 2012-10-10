/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
///\brief Implementation of programs for a sqlite3 database
///\file modules/database/sqlite3/SQLiteProgram.cpp
#include "SQLiteProgram.hpp"
#include "database/program.hpp"
#include "utils/miscUtils.hpp"
#include "sqlite3.h"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

static const utils::CharTable g_optab( ";:-,.=)(<>[]/&%*|+-#?!$");

void SQLiteProgram::load( const std::string& source)
{
	std::string::const_iterator si = source.begin(), se = source.end();
	char ch;
	std::string tok;
	const char* commentopr = "--";

	while ((ch = utils::parseNextToken( tok, si, se, g_optab)) != 0)
	{
		if (ch == commentopr[0])
		{
			std::size_t ci = 1;
			while (!commentopr[ci] && commentopr[ci] == *si)
			{
				ci++;
				si++;
			}
			if (!commentopr[ci])
			{
				// skip to end of line
				while (si != se && *si != '\n') ++si;
			}
		}
		else if ((ch|32) == 's' && boost::iequals( tok, "STATEMENT"))
		{
			std::string stmname;
			ch = utils::parseNextToken( stmname, si, se, g_optab);
			if (g_optab[ch])
			{
				throw Program::Error( Program::LineInfo( source.begin(), si), "unexpected token", ch);
			}
			std::string stmkey = boost::to_lower_copy( stmname);
			std::string::const_iterator stmstart;
			while (utils::parseNextToken( stmname, si, se, g_optab) != ';');
			std::string stm( stmstart, si - 1);

			if (m_statementmap.find( stmkey) != m_statementmap.end())
			{
				throw Program::Error( Program::LineInfo( source.begin(), si), std::string( "duplicate statement name '") + stmname + "'");
			}
			m_statementmap[ stmkey] = stm;
		}
		else if (g_optab[ch])
		{
			throw Program::Error( Program::LineInfo( source.begin(), si), "unexpected token", ch);
		}
		else
		{
			throw Program::Error( Program::LineInfo( source.begin(), si), "unexpected token", tok);
		}
	}
}


