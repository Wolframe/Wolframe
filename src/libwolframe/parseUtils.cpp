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
///\file parseUtils.cpp
///\brief Some utility functions for reading and parsing source files

#include "utils/parseUtils.hpp"
#include <cstring>
#include <cstdio>
#include <sstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <stdarg.h>

using namespace _Wolframe;
using namespace _Wolframe::utils;

CharTable::CharTable( const char* op, bool isInverse)
{
	std::size_t ii;
	for (ii=0; ii<=32; ++ii) m_ar[ii] = false;
	for (ii=33; ii<sizeof(m_ar); ++ii) m_ar[ii] = isInverse;
	for (ii=0; op[ii]; ++ii)
	{
		if (op[ii] == '.' && op[ii+1] == '.' && op[ii+2] && ii > 0)
		{
			unsigned char hi = (unsigned char)op[ii+2];
			unsigned char lo = (unsigned char)op[ii-1];
			if (hi < lo)
			{
				unsigned char tmp = hi;
				hi = lo;
				lo = tmp; //... swapped 'hi' and 'lo'
			}
			for (++lo; lo<=hi; ++lo)
			{
				m_ar[ lo] = !isInverse;
			}
			ii += 2;
		}
		m_ar[(unsigned char)(op[ii])] = !isInverse;
	}
}

char utils::parseNextToken( std::string& tok, std::string::const_iterator& itr, std::string::const_iterator end, const CharTable& operatorTable, const CharTable& alphaTable)
{
	char rt = '\0';
	tok.clear();
	while (itr != end && (unsigned char)*itr <= 32) ++itr;
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
		++itr;
		return rt;
	}
	else if (alphaTable[ *itr])
	{
		while (itr != end && (unsigned char)*itr > 32 && alphaTable[ *itr]) tok.push_back( *itr++);
		return rt;
	}
	else
	{
		throw std::runtime_error( std::string( "unexpected token character '") + *itr + "'");
	}
}

struct IdentifierCharTable :public CharTable
{
	IdentifierCharTable() :CharTable( "a..z_A..Z0..9"){}
};

const CharTable& _Wolframe::utils::identifierCharTable()
{
	static const IdentifierCharTable rt;
	return rt;
}

const CharTable& _Wolframe::utils::emptyCharTable()
{
	static CharTable rt;
	return rt;
}

const CharTable& _Wolframe::utils::anyCharTable()
{
	static CharTable rt( "", true);
	return rt;
}

char _Wolframe::utils::parseNextToken( std::string& tok, std::string::const_iterator& itr, std::string::const_iterator end, const CharTable& operatorTable)
{
	return parseNextToken( tok, itr, end, operatorTable, identifierCharTable());
}

char _Wolframe::utils::parseNextToken( std::string& tok, std::string::const_iterator& itr, std::string::const_iterator end)
{
	return parseNextToken( tok, itr, end, emptyCharTable());
}

char _Wolframe::utils::gotoNextToken( std::string::const_iterator& itr, std::string::const_iterator end)
{
	while (itr != end && (unsigned char)*itr <= 32) ++itr;
	return (itr == end)?0:*itr;
}

std::string _Wolframe::utils::parseNextLine( std::string::const_iterator& itr, std::string::const_iterator end)
{
	std::string rt;
	for (; itr != end && *itr != '\n'; ++itr)
	{
		if (*itr == '\\')
		{
			++itr;
			if (itr == end) throw std::runtime_error( "found '\\' at end of input");
			if (*itr != '\n')
			{
				rt.push_back( '\\');
				rt.push_back( *itr);
				std::string::const_iterator next = itr;
				for (;next != end && *next != '\n' && (unsigned char)*next <= 32; ++next);
				if (next != end && *next == '\n') std::runtime_error( "found spaces after a '\\' at end of a line");
			}
		}
		else
		{
			rt.push_back( *itr);
		}
	}
	if (itr != end) ++itr;
	return rt;
}

std::pair<std::string,std::string> _Wolframe::utils::parseTokenAssignement( std::string::const_iterator& itr, std::string::const_iterator end, const CharTable& alphaTable)
{
	std::pair<std::string,std::string> rt;
	static CharTable optab("=");

	switch (utils::parseNextToken( rt.first, itr, end, optab))
	{
		case '\0':
		case '=': std::runtime_error( "identifier expected of assignment before '='");
		default:
			if (utils::gotoNextToken( itr, end) != '=')
			{
				std::runtime_error( "'=' expected after identifier of assignment");
			}
			++itr;
			if (utils::parseNextToken( rt.second, itr, end, optab, alphaTable) == '\0')
			{
				std::runtime_error( "unexpected end of file in attribute '!name=..' definition");
			}
	}
	return rt;
}

std::pair<std::string,std::string> utils::parseTokenAssignement( std::string::const_iterator& itr, std::string::const_iterator end)
{
	return parseTokenAssignement( itr, end, identifierCharTable());
}

LineInfo utils::getLineInfo( const std::string::const_iterator& start, const std::string::const_iterator& pos)
{
	LineInfo rt;
	std::string::const_iterator ii = start;
	for (; ii!=pos; ++ii)
	{
		if (*ii == '\n')
		{
			rt.incrementLine();
		}
		else if (*ii != '\r')
		{
			rt.incrementColumn();
		}
	}
	return rt;
}

std::string utils::parseLine( std::string::const_iterator& si, const std::string::const_iterator& se)
{
	std::string rt;
	while (si != se && *si != '\n' && *si != '\r')
	{
		rt.push_back( *si);
		++si;
	}
	if (si != se)
	{
		if (*si == '\r')
		{
			++si;
			if (si != se && *si == '\n') ++si;
		}
		else
		{
			++si;
		}
	}
	return rt;
}

IdentifierTable::IdentifierTable( bool casesensitive_, const char** arg)
	:m_casesensitive(casesensitive_)
	,m_arg(arg)
{
	for (int idx=0; arg[idx]; ++idx)
	{
		if (idx >= 100) throw std::logic_error("internal: IdentifierTable parameter list not NULL terminated or with more than 100 elements");
		std::string kk( arg[idx]);
		if (!m_casesensitive) boost::algorithm::to_upper( kk);
		m_tab[ kk] = idx+1;
	}
}

int IdentifierTable::operator[]( const std::string& tok) const
{
	std::map<std::string,int>::const_iterator ti;
	if (m_casesensitive)
	{
		ti = m_tab.find( tok);
	}
	else
	{
		ti = m_tab.find( boost::algorithm::to_upper_copy( tok));
	}
	if (ti == m_tab.end()) return 0;
	return ti->second;
}

std::string IdentifierTable::tostring() const
{
	std::string rt;
	std::map<std::string,int>::const_iterator ti = m_tab.begin(), te = m_tab.end();
	for (int idx=0; ti != te; ++ti)
	{
		if (idx++) rt.append(", ");
		rt.append( ti->first);
	}
	return rt;
}

const char* IdentifierTable::idstring( int id) const
{
	return m_arg[ id];
}

int utils::parseNextIdentifier( std::string::const_iterator& si, const std::string::const_iterator& se, const IdentifierTable& idtab)
{
	char ch = utils::gotoNextToken( si, se);
	if (identifierCharTable()[ch])
	{
		std::string::const_iterator start = si;
		std::string tok;
		utils::parseNextToken( tok, si, se);
		int idx = idtab[ tok];
		if (idx) return idx;
		si = start;
	}
	return 0;
}

