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
///\brief Definition of autorization for a transaction function
///\file tdl/authorizeDefinition.hpp
#include "tdl/authorizeDefinition.hpp"
#include "tdl/parseUtils.hpp"
#include <string>
#include <vector>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::tdl;

AuthorizeDefinition AuthorizeDefinition::parse( const LanguageDescription* langdescr, std::string::const_iterator& si, std::string::const_iterator se)
{
	std::string tok;

	std::string authfunction;
	std::string authresource;

	char ch = gotoNextToken( langdescr, si, se);
	if (ch != '(') throw std::runtime_error( "Open bracket '(' expected after AUTHORIZE function call");
	si++;
	if (!parseNextToken( langdescr, authfunction, si, se))
	{
		throw std::runtime_error( "unexpected end of description. function name expected after AUTHORIZE");
	}
	if (authfunction.empty())
	{
		throw std::runtime_error( "AUTHORIZE function name must not be empty");
	}
	ch = gotoNextToken( langdescr, si, se);
	if (ch == ',')
	{
		++si;
		if (!parseNextToken( langdescr, authresource, si, se))
		{
			throw std::runtime_error( "unexpected end of description. resource name expected as argument of AUTHORIZE function call");
		}
	}
	ch = gotoNextToken( langdescr, si, se);
	if (ch != ')')
	{
		throw std::runtime_error( "Close bracket ')' expected after AUTHORIZE function defintion");
	}
	++si;
	return AuthorizeDefinition( authfunction, authresource);
}

