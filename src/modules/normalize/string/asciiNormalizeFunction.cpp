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
///\file asciiNormalizeFunction.cpp
#include "asciiNormalizeFunction.hpp"

using namespace _Wolframe;
using namespace langbind;

std::string AsciiNormalizeFunction::execute( const std::string& str) const
{
	static const char hex[] = "0123456789abcdef";
	std::string::const_iterator ii = str.begin(), ee = str.end();
	while (ii != ee && *ii <= 32 && *ii >= 0) ++ii;
	std::string rt;
	for (; ii != ee; ++ii)
	{
		if ((unsigned char)*ii > 32)
		{
			if (*ii >= '0' && *ii <= '9') rt.push_back( *ii);
			if (*ii >= 'a' && *ii <= 'z') rt.push_back( std::toupper( *ii));
			if (*ii >= 'A' && *ii <= 'Z') rt.push_back( *ii);
			else
			{
				unsigned char xx = (unsigned char)*ii;
				rt.push_back( '#');
				rt.push_back( hex[ xx/16]);
				rt.push_back( hex[ xx%16]);
			}
		}
		else if (!rt.empty() && rt[rt.size()-1] != ' ')
		{
			rt.push_back( ' ');
		}
	}
	if (!rt.empty() && rt[rt.size()-1] == ' ')
	{
		rt.resize(rt.size()-1);
	}
	return rt;
}

