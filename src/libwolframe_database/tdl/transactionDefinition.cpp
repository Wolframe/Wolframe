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
///\brief Definition of a transaction
///\file tdl/transactionDefinition.hpp
#include "database/databaseLanguage.hpp"
#include "tdl/transactionDefinition.hpp"
#include "tdl/parseUtils.hpp"
#include "utils/parseUtils.hpp"
#include <string>
#include <vector>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::tdl;

ResultDefinition ResultDefinition::parse( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	ResultDefinition rt;
	std::string tok;
	char ch;
	unsigned int mask = 0;

	std::string::const_iterator start = si;
	while (0!=(ch=parseNextToken( langdescr, tok, si, se)))
	{
		if (isAlpha(ch) && boost::algorithm::iequals( tok, "INTO"))
		{
			if ((mask & 0x2) != 0) throw std::runtime_error( "wrong order of definition in RESULT: INTO defined after FILTER");
			if ((mask & 0x1) != 0) throw std::runtime_error( "duplicate INTO definition after RESULT");
			mask |= 0x1;
			rt.path = parse_INTO_path( langdescr, si, se);
		}
		else if (isAlpha(ch) && boost::algorithm::iequals( tok, "FILTER"))
		{
			if ((mask & 0x2) != 0) throw std::runtime_error( "duplicate FILTER definition after RESULT");
			mask |= 0x2;
			ch = gotoNextToken( langdescr, si, se);
			if (!isAlpha(ch))
			{
				throw std::runtime_error( "function name expected after RESULT FILTER");
			}
			rt.filter = parseFunctionName( langdescr, si, se);
		}
		else if (mask)
		{
			si = start;
			break;
		}
		else
		{
			throw std::runtime_error( "INTO or FILTER expected after RESULT");
		}
		start = si;
	}
	return rt;
}

