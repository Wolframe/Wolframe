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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file utils_conversions.cpp
///\brief Implementation of conversions of atomic types

#include <stdexcept>
#include "utils/conversions.hpp"

using namespace _Wolframe;
using namespace _Wolframe::utils;
namespace {
class NumTable
{
public:
	NumTable()
	{
		std::size_t ii=0,dd=0;
		for (; ii<100; ++ii,dd+=2)
		{
			tab.chr[dd+0] = (char)((ii/10) + '0');
			tab.chr[dd+1] = (char)((ii%10) + '0');
		}
	}

	struct Number64
	{
		union
		{
			char chr[22];
			short ar[11];
		} tab;
	};

	const char* getNumberString( _WOLFRAME_UINTEGER val, Number64& str) const
	{
		std::size_t ii=0;
		if (val == 0) return "0";
		str.tab.chr[ 20] = '\0';
		for (; ii<10 && val>0; ++ii)
		{
			std::size_t ai = val % 100;
			val /= 100;

			str.tab.ar[ 9-ii] = tab.ar[ ai];
		}
		std::size_t ni = 20-(ii<<1);
		ni += (str.tab.chr[ ni] == '0');
		return str.tab.chr + ni;
	}

private:
	union
	{
		char chr[200];
		short ar[100];
	} tab;
};
}//anonymous namespace

static void get_number_string( std::string& rt, _WOLFRAME_UINTEGER val)
{
	static const NumTable tab;
	NumTable::Number64 numstrbuf;
	rt.append( tab.getNumberString( val, numstrbuf));
}

static void get_number_string( std::string& rt, _WOLFRAME_INTEGER val)
{
	static const NumTable tab;
	NumTable::Number64 numstrbuf;
	if (val < 0)
	{
		rt.push_back( '-');
		rt.append( tab.getNumberString( (_WOLFRAME_UINTEGER)-val, numstrbuf));
	}
	else
	{
		rt.append( tab.getNumberString( (_WOLFRAME_UINTEGER)val, numstrbuf));
	}
}

static _WOLFRAME_UINTEGER string2uint( const char *pp)
{
	_WOLFRAME_UINTEGER rt = 0, prev = 0;
	while (*pp >= '0' && *pp <= '9')
	{
		prev = rt;
		rt = (rt*10) + (*pp - '0');
		++pp;
		if (rt < prev) throw std::runtime_error( "string to number conversion error: value out of range");
	}
	if (*pp != '\0') throw std::runtime_error( "string to number conversion error: value is not a sequence of digits");
	return rt;
}

std::string utils::tostring_cast( _WOLFRAME_UINTEGER val)
{
	std::string rt;
	get_number_string( rt, val);
	return rt;
}

std::string utils::tostring_cast( _WOLFRAME_INTEGER val)
{
	std::string rt;
	get_number_string( rt, val);
	return rt;
}

_WOLFRAME_UINTEGER utils::touint_cast( const std::string& val)
{
	if (val.empty()) throw std::runtime_error( "string to number conversion error: string empty");
	return string2uint( val.c_str());
}

_WOLFRAME_INTEGER utils::toint_cast( const std::string& val)
{
	if (val.empty()) throw std::runtime_error( "string to number conversion error: string empty");
	if (val.at(0) == '-')
	{
		_WOLFRAME_UINTEGER rt = string2uint( val.c_str()+1);
		if( rt > (_WOLFRAME_UINTEGER)std::numeric_limits<_WOLFRAME_INTEGER>::max( ) + 1 ) throw std::runtime_error( "string to integer conversion error: value out of range");
		return -(_WOLFRAME_INTEGER)(rt);
	}
	else
	{
		_WOLFRAME_INTEGER rt = (_WOLFRAME_INTEGER)string2uint( val.c_str());
		if (rt < 0) throw std::runtime_error( "string to integer conversion error: value out of range");
		return rt;
	}
}


