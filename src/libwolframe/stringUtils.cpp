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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file stringUtils.cpp
///\brief Some utility string function implementations. They are mainly boost wrappers that are separated because of some warnings needed to be disabled

#ifdef _WIN32
#pragma warning(disable:4996)
#endif
#include "utils/miscUtils.hpp"
#include <cstring>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe::utils;

void _Wolframe::utils::splitString( std::vector<std::string>& res, const std::string& inp, const char* splitchrs)
{
	res.clear();
	std::vector<std::string> imm;
	boost::split( imm, inp, boost::is_any_of(splitchrs));
	std::vector<std::string>::const_iterator vi=imm.begin(), ve=imm.end();
	for (; vi != ve; ++vi) if (!vi->empty()) res.push_back( *vi);
}

OperatorTable::OperatorTable( const char* op)
{
	std::size_t ii;
	for (ii=0; ii<sizeof(m_ar); ++ii) m_ar[ii]=false;
	for (ii=0; op[ii]; ++ii) m_ar[(unsigned char)(op[ii])]=true;
}

static bool isLetter( char ch)
{
	return (ch < 0 || ((ch|32) >= 'a' && (ch|32) <= 'z') || (ch >= '0' && ch <= '9') || ch == '_');
}

char _Wolframe::utils::parseNextToken( std::string& tok, std::string::const_iterator& itr, std::string::const_iterator end, const OperatorTable& operatorTable)
{
	char rt = '\0';
	tok.clear();
	while (*itr <= 32 && *itr >= 0 && itr != end) ++itr;
	if (itr == end) return '\0';
	rt = *itr;
	if (*itr == '\'' || *itr == '\"')
	{
		char eb = *itr;
		for (++itr; itr != end; ++itr)
		{
			if (*itr == eb)
			{
				++itr;
				return rt;
			}
			else if (*itr == '\\')
			{
				++itr;
				if (itr == end) throw std::runtime_error( "string not terminated");
			}
			if (*itr == 0) throw std::runtime_error( "string has multibyte encoding or is binary");
			tok.push_back( *itr);
		}
		throw std::runtime_error( "string not terminated");
	}
	else if (operatorTable[ *itr])
	{
		tok.push_back( *itr);
		++itr;
		return rt;
	}
	else if (isLetter( *itr))
	{
		while (isLetter( *itr)) tok.push_back( *itr++);
		return rt;
	}
	else
	{
		throw std::runtime_error( std::string( "illegal token character '") + *itr + "'");
	}
}

char _Wolframe::utils::parseNextToken( std::string& tok, std::string::const_iterator& itr, std::string::const_iterator end)
{
	static OperatorTable noOperator;
	return parseNextToken( tok,itr,end,noOperator);
}
