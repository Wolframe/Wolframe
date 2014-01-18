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
///\file asciiNormalizeFunction.cpp
#include "asciiNormalizeFunction.hpp"
#include "textwolf/charset_utf8.hpp"
#include "textwolf/istreamiterator.hpp"
#include "types/variant.hpp"
#include "utils/parseUtils.hpp"
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <iostream>

using namespace _Wolframe;
using namespace langbind;

static void substDia_europe( std::istream& input, std::ostream& output)
{
	textwolf::charset::UTF8 utf8;
	char buf[16];
	unsigned int bufpos;
	textwolf::IStreamIterator itr(input);
	while (*itr)
	{
		bufpos = 0;
		textwolf::UChar value = utf8.value( buf, bufpos, itr);

		if (value < 0x7F)
		{
			output << (char)value;
		}
		else if (value >= 0xC0 && value <= 0xC5)
		{
			output << 'A';
		}
		else if (value == 0xC6)
		{
			output << "AE";
		}
		else if (value == 0xC7)
		{
			output << 'C';
		}
		else if (value >= 0xC8 && value <= 0xCB)
		{
			output << 'E';
		}
		else if (value >= 0xCC && value <= 0xCF)
		{
			output << 'I';
		}
		else if (value == 0xD0)//CAPITAL ETH
		{
			output << "TH";
		}
		else if (value == 0xD1)
		{
			output << 'N';
		}
		else if (value >= 0xD2 && value <= 0xD6)
		{
			output << 'O';
		}
		else if (value == 0xD7)
		{
			output << '*';
		}
		else if (value == 0xD8)
		{
			output << 'O';
		}
		else if (value >= 0xD9 && value <= 0xDC)
		{
			output << 'U';
		}
		else if (value == 0xDD)
		{
			output << 'Y';
		}
		else if (value == 0xDE)//CAPITAL THORN
		{
			output << "TH";
		}
		else if (value == 0xDF)
		{
			output << "ss";
		}
		else if (value >= 0xE0 && value <= 0xE5)
		{
			output << 'a';
		}
		else if (value == 0xE6)
		{
			output << "ae";
		}
		else if (value == 0xE7)
		{
			output << 'c';
		}
		else if (value >= 0xE8 && value <= 0xEB)
		{
			output << 'e';
		}
		else if (value >= 0xEC && value <= 0xEF)
		{
			output << 'i';
		}
		else if (value == 0xF0)//SMALL ETH
		{
			output << "th";
		}
		else if (value == 0xF1)
		{
			output << 'n';
		}
		else if (value >= 0xF2 && value <= 0xF6)
		{
			output << 'o';
		}
		else if (value == 0xF7)
		{
			output << '/';
		}
		else if (value == 0xF8)
		{
			output << 'o';
		}
		else if (value >= 0xF9 && value <= 0xFC)
		{
			output << 'u';
		}
		else if (value == 0xFD)
		{
			output << 'y';
		}
		else if (value == 0xFE)
		{
			output << "th";
		}
		else if (value == 0xFF)
		{
			output << 'y';
		}
		else if (value == 0x1E9E)
		{
			output << "SS";
		}
		//romanian characters above 0xFF:
		else if (value == 0x102)
		{
			output << 'A';
		}
		else if (value == 0x103)
		{
			output << 'a';
		}
		else if (value == 0x218 || value == 0x15F)
		{
			output << 'S';
		}
		else if (value == 0x219 || value == 0x15E)
		{
			output << 's';
		}
		else if (value == 0x21A || value == 0x162)
		{
			output << 'T';
		}
		else if (value == 0x21B || value == 0x163)
		{
			output << 't';
		}
	}
}

types::Variant ConvDiaNormalizeFunction::execute( const types::Variant& inp) const
{
	std::istringstream input( inp.tostring());
	std::ostringstream output;

	substDia_europe( input, output);
	return output.str();
}

static std::string nameString( const std::string& str)
{
	const utils::CharTable optab( ";:-,.=)(<>[]{}/&%*|+-#?!$");
	std::istringstream input( str);
	std::ostringstream output;

	substDia_europe( input, output);
	std::string asciistr = output.str();
	std::string::const_iterator ii = asciistr.begin(), ee = asciistr.end();

	while (ii != ee && *ii <= 32 && *ii >= 0) ++ii;
	std::string rt;
	for (; ii != ee; ++ii)
	{
		if ((unsigned char)*ii > 32 && !optab[(unsigned char)*ii])
		{
			if (*ii >= '0' && *ii <= '9') rt.push_back( *ii);
			if (*ii >= 'a' && *ii <= 'z') rt.push_back( *ii);
			if (*ii >= 'A' && *ii <= 'Z') rt.push_back( *ii);
			if (*ii == '_') rt.push_back( *ii);
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

types::Variant UppercaseNameNormalizeFunction::execute( const types::Variant& inp) const
{
	return boost::to_upper_copy( nameString( inp.tostring()));
}

types::Variant LowercaseNameNormalizeFunction::execute( const types::Variant& inp) const
{
	return boost::to_lower_copy( nameString( inp.tostring()));
}

