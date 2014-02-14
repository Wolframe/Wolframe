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
///\file databaseLanguage.cpp
///\brief Implementation of language definitions for embedded commands in transactions
#include "database/databaseLanguage.hpp"
#include "utils/parseUtils.hpp"
#include "types/keymap.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

std::string LanguageDescription::stm_argument_reference( int index) const
{
	std::ostringstream rt;
	rt << "$" << index;
	return rt.str();
}

namespace {
class SQL_KeywordMap :public types::keymap<bool>
{
public:
	SQL_KeywordMap& operator()( const char* keyword)
	{
		types::keymap<bool>::insert( std::string(keyword), true);
		return *this;
	}

	SQL_KeywordMap()
	{
		(*this)("INTO")("FROM")("SELECT")("UPDATE")("DELETE")("CREATE")
		("INSERT")("RENAME")("REPLACE")("ORDER")("GROUP")("IN")("BY")
		("ON")("OF")("DESC")("ASC")("DISTINCT")("DROP")("INNER")("OUTER")
		("NATURAL")("INSTEAD")("INTERSECT")("IS")("JOIN")("LEFT")("LIKE")
		("LIMIT")("MATCH")("NULL")("SET")("TABLE")("TEMPORARY")
		("UNION")("UNIQUE")("VALUES")("VIEW")("AND")("OR")("NOT");
	}
};
}

static SQL_KeywordMap g_keywordMap;


bool LanguageDescription::isEmbeddedStatement( std::string::const_iterator si, std::string::const_iterator se) const
{
	while (si != se && (*si > 0 && *si <= 32)) ++si;
	std::string::const_iterator start = si;
	while (si != se && ((*si >= 'a' && *si <= 'z') || (*si >= 'A' && *si <= 'Z'))) ++si;
	if (si != se && (*si > 0 && *si <= 32))
	{
		std::string keyword = boost::algorithm::to_lower_copy( std::string( start, si));
		if (keyword == "select"
		|| keyword == "update"
		|| keyword == "insert"
		|| keyword == "delete"
		|| keyword == "create"
		|| keyword == "drop")
		{
			while (si != se && (*si > 0 && *si <= 32)) ++si;
			return (si != se && (*si == '"' || *si == '\'' || (*si >= 'a' && *si <= 'z') || (*si >= 'A' && *si <= 'Z') || *si == '_' || *si == '$'  || *si == '*' || (*si >= '0' && *si <= '9')));
		}
	}
	return false;
}

static const utils::CharTable g_optab( ";:-,.=)(<>[]{}/&%*|+-#?!$");

std::string LanguageDescription::parseEmbeddedStatement( std::string::const_iterator& si, std::string::const_iterator se) const
{
	std::string rt;
	std::string tok;
	const char* commentopr = eoln_commentopr();
	std::string::const_iterator start = si;
	char ch;
	while (0!=(ch=utils::parseNextToken( tok, si, se, g_optab)))
	{
		if (ch == ';') break;
		if (ch == commentopr[0])
		{
			std::string::const_iterator ti = si;
			std::size_t ci = 1;
			while (commentopr[ci] && ti != se && commentopr[ci] == *ti)
			{
				++ci;
				++ti;
			}
			if (!commentopr[ci])
			{
				// skip to end of line
				while (ti != se && *ti != '\n') ++ti;
				rt.append( std::string( start, si-1));
				start = si = ti;
				continue;
			}
		}
	}
	if (ch != ';')
	{
		throw std::runtime_error( "embedded statement not terminated");
	}
	--si; //... terminating ';' is not swallowed
	rt.append( std::string( start, si));
	return rt;
}

static bool isAlpha( char ch)
{
	if (ch >= 'A' && ch <= 'Z') return true;
	if (ch >= 'a' && ch <= 'z') return true;
	if (ch == '_') return true;
	return false;
}

std::string LanguageDescription::substituteTemplateArguments( const std::string& cmd, const std::vector<TemplateArgumentAssignment>& arg) const
{
	const char* commentopr = eoln_commentopr();
	std::vector<TemplateArgumentAssignment>::const_iterator ai = arg.begin(), ae = arg.end();
	for (; ai != ae; ++ai)
	{
		if (g_keywordMap.find( ai->first) != g_keywordMap.end())
		{
			throw std::runtime_error( "tried to use SQL keyword as template argument");
		}
	}
	std::string rt;
	std::string tok;
	std::string::const_iterator si = cmd.begin(), se = cmd.end();
	std::string::const_iterator start = si;
	char ch;
	while (0!=(ch=utils::parseNextToken( tok, si, se, g_optab)))
	{
		if (isAlpha(ch))
		{
			ai = arg.begin(), ae = arg.end();
			for (; ai != ae; ++ai)
			{
				if (boost::algorithm::iequals( tok, ai->first))
				{
					// ... substiture identifier
					rt.append( start, si - tok.size());
					rt.append( ai->second);
					start = si;
				}
			}
		}
		else if (ch == commentopr[0])
		{
			std::size_t ci = 1;
			while (commentopr[ci] && si != se && commentopr[ci] == *si)
			{
				++ci;
				++si;
			}
			if (!commentopr[ci])
			{
				// skip to end of line
				while (si != se && *si != '\n') ++si;
			}
		}
	}
	if (start == cmd.begin())
	{
		return cmd;
	}
	else
	{
		rt.append( start, si);
		return rt;
	}
}

