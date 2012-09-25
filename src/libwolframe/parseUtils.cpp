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
///\file parseUtils.cpp
///\brief Some utility functions for reading and parsing source files

#include "utils/miscUtils.hpp"
#include <cstring>
#include <sstream>
#include <string>
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/exception.hpp>

using namespace _Wolframe::utils;

CharTable::CharTable( const char* op, bool isInverse)
{
	std::size_t ii;
	for (ii=0; ii<sizeof(m_ar); ++ii) m_ar[ii]=isInverse;
	for (ii=0; op[ii]; ++ii) m_ar[(unsigned char)(op[ii])]=!isInverse;
}

char _Wolframe::utils::parseNextToken( std::string& tok, std::string::const_iterator& itr, std::string::const_iterator end, const CharTable& operatorTable, const CharTable& alphaTable)
{
	char rt = '\0';
	tok.clear();
	while (itr != end && *itr <= 32 && *itr >= 0) ++itr;
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
		throw std::runtime_error( std::string( "illegal token character '") + *itr + "'");
	}
}

struct IdentifierTable :public CharTable
{
	IdentifierTable() :CharTable( "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"){}
};

char _Wolframe::utils::parseNextToken( std::string& tok, std::string::const_iterator& itr, std::string::const_iterator end, const CharTable& operatorTable)
{
	static CharTable identifierTable( "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	return parseNextToken( tok, itr, end, operatorTable, identifierTable);
}

char _Wolframe::utils::parseNextToken( std::string& tok, std::string::const_iterator& itr, std::string::const_iterator end)
{
	static CharTable noOperator;
	return parseNextToken( tok, itr, end, noOperator);
}

char _Wolframe::utils::gotoNextToken( std::string::const_iterator& itr, std::string::const_iterator end)
{
	while (itr != end && *itr <= 32) ++itr;
	return (itr == end)?0:*itr;
}

template <class Result>
static void readSourceFileContentT( const std::string& filename, Result& res)
{
	std::ifstream inFile( filename.c_str());
	try
	{
		inFile.exceptions( std::ifstream::failbit | std::ifstream::badbit);

		while (inFile)
		{
			std::string ln;
			if (inFile.eof()) break;
			std::getline( inFile, ln);
			res.push_back( ln);
		}
	}
	catch (const std::ifstream::failure& e)
	{
		if (!(inFile.rdstate() & std::ifstream::eofbit))
		{
			std::ostringstream msg;
			msg << "error '" << e.what() << "' reading file '" << filename << "'" << std::endl;
			throw std::runtime_error( msg.str());
		}
	}
}

struct ContentBuffer :public std::ostringstream
{
	void push_back( const std::string& line)
	{
		(*this) << line << std::endl;
	}
};

std::string _Wolframe::utils::readSourceFileContent( const std::string& filename)
{
	ContentBuffer rt;
	readSourceFileContentT<ContentBuffer>( filename, rt);
	return rt.str();
}

std::vector<std::string> _Wolframe::utils::readSourceFileLines( const std::string& filename)
{
	std::vector<std::string> rt;
	readSourceFileContentT( filename, rt);
	return rt;
}


