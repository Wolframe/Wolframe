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
///\brief Definition of embedded database statement parsing result
///\file tdl/embeddedStatement.hpp
#ifndef _DATABASE_TDL_EMBEDDED_STATEMENT_HPP_INCLUDED
#define _DATABASE_TDL_EMBEDDED_STATEMENT_HPP_INCLUDED
#include "database/databaseLanguage.hpp"
#include "tdl/elementReference.hpp"
#include <string>
#include <vector>
#include <stdexcept>

namespace _Wolframe {
namespace db {
namespace tdl {

struct EmbeddedStatement
{
	std::string stmstring;
	std::vector<ElementReference> params;

	EmbeddedStatement(){}
	EmbeddedStatement( const EmbeddedStatement& o)
		:stmstring(o.stmstring),params(o.params){}
	EmbeddedStatement( const std::string& stmstring_, const std::vector<ElementReference>& params_)
		:stmstring(stmstring_),params(params_){}

	static EmbeddedStatement parse( const LanguageDescription* langdescr, std::string::const_iterator& si, std::string::const_iterator se);
};

}}}//namespace
#endif

