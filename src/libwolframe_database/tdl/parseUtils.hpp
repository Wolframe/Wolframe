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
///\brief Utility functions for parsing
///\file tdl/elementReference.hpp
#ifndef _DATABASE_TDL_PARSE_UTILS_HPP_INCLUDED
#define _DATABASE_TDL_PARSE_UTILS_HPP_INCLUDED
#include "database/databaseLanguage.hpp"
#include "utils/parseUtils.hpp"
#include <string>
#include <vector>
#include <cstdlib>
#include <stdexcept>

namespace _Wolframe {
namespace db {
namespace tdl {

static inline bool isAlpha( char ch)
{
	if (ch >= 'A' && ch <= 'Z') return true;
	if (ch >= 'a' && ch <= 'z') return true;
	if (ch == '_') return true;
	return false;
}

static inline bool isAlphaNumeric( char ch)
{
	if (ch >= '0' && ch <= '9') return true;
	return isAlpha( ch);
}

static inline bool isDigit( char ch)
{
	return (ch >= '0' && ch <= '9');
}

bool isIdentifier( const std::string& str);
std::string errorTokenString( char ch, const std::string& tok);
char gotoNextToken( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator se);
char parseNextToken( const LanguageDescription* langdescr, std::string& tok, std::string::const_iterator& si, std::string::const_iterator se);

std::vector<std::string> parse_INTO_path( const LanguageDescription* langdescr, std::string::const_iterator& si, std::string::const_iterator se);
std::vector<std::string> parseTemplateArguments( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se);

void checkUniqOccurrence( int id, unsigned int& mask, const utils::IdentifierTable& idtab);

}}}//namespace
#endif

