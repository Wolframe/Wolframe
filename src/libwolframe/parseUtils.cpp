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

#include "utils/miscUtils.hpp"
#include <cstring>
#include <cstdio>
#include <sstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>

using namespace _Wolframe::utils;

CharTable::CharTable( const char* op, bool isInverse)
{
	std::size_t ii;
	for (ii=0; ii<=32; ++ii) m_ar[ii]=false;
	for (ii=33; ii<sizeof(m_ar); ++ii) m_ar[ii]=isInverse;
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
		throw std::runtime_error( std::string( "unexpected token character '") + *itr + "'");
	}
}

struct IdentifierTable :public CharTable
{
	IdentifierTable() :CharTable( "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"){}
};

const CharTable& _Wolframe::utils::identifierCharTable()
{
	static const IdentifierTable rt;
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
	while (itr != end && *itr <= 32) ++itr;
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
				for (;next != end && *next != '\n' && *next > 0 && *next <= 32; ++next);
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

std::pair<std::string,std::string> _Wolframe::utils::parseTokenAssignement( std::string::const_iterator& itr, std::string::const_iterator end)
{
	return parseTokenAssignement( itr, end, identifierCharTable());
}

std::pair<unsigned int,unsigned int> _Wolframe::utils::getLineInfo( const std::string::const_iterator& start, const std::string::const_iterator& pos)
{
	std::pair<unsigned int,unsigned int> rt(1,1);
	std::string::const_iterator ii = start;
	for (; ii!=pos; ++ii)
	{
		if (*ii == '\n')
		{
			rt.second = 0;
			rt.first += 1;
			continue;
		}
		++rt.second;
	}
	return rt;
}

static void readFileContent( const std::string& filename, std::string& res)
{
	unsigned char ch;
	FILE* fh = fopen( filename.c_str(), "r");
	if (!fh)
	{
		throw std::runtime_error( std::string( "failed (errno " + boost::lexical_cast<std::string>(errno) + ") to open file ") + filename + "' for reading");
	}
	boost::shared_ptr<FILE> fhr( fh, fclose);
	while (1 == fread( &ch, 1, 1, fh))
	{
		res.push_back( ch);
	}
	if (!feof( fh))
	{
		int ec = ferror( fh);
		if (ec) throw std::runtime_error( std::string( "failed to read (errno " + boost::lexical_cast<std::string>(ec) + ") from file ") + filename + "'");
	}
}

static void readSourceFileLines_( const std::string& filename, std::vector<std::string>& res)
{
	unsigned char ch;
	std::string ln;
	FILE* fh = fopen( filename.c_str(), "r");
	if (!fh)
	{
		throw std::runtime_error( std::string( "failed (errno " + boost::lexical_cast<std::string>(errno) + ") to open file ") + filename + "' for reading");
	}
	boost::shared_ptr<FILE> fhr( fh, fclose);
	while (1 == fread( &ch, 1, 1, fh))
	{
		if (ch == '\n')
		{
			res.push_back( ln);
			ln.clear();
		}
		else
		{
			ln.push_back( ch);
		}
	}
	if (!feof( fh))
	{
		int ec = ferror( fh);
		if (ec) throw std::runtime_error( std::string( "failed to read (errno " + boost::lexical_cast<std::string>(ec) + ") from file ") + filename + "'");
	}
}

void _Wolframe::utils::writeFile( const std::string& filename, const std::string& content)
{
	unsigned char ch;
	FILE* fh = fopen( filename.c_str(), "w");
	if (!fh)
	{
		throw std::runtime_error( std::string( "failed (errno " + boost::lexical_cast<std::string>(errno) + ") to open file ") + filename + "' for reading");
	}
	boost::shared_ptr<FILE> fhr( fh, fclose);
	std::string::const_iterator fi = content.begin(), fe = content.end();
	for (; fi != fe; ++fi)
	{
		ch = *fi;
		if (1 > fwrite( &ch, 1, 1, fh))
		{
			int ec = ferror( fh);
			if (ec) throw std::runtime_error( std::string( "failed to read (errno " + boost::lexical_cast<std::string>(ec) + ") from file ") + filename + "'");
		}
	}
}

std::string _Wolframe::utils::readSourceFileContent( const std::string& filename)
{
	std::string rt;
	readFileContent( filename, rt);
	return rt;
}

std::vector<std::string> _Wolframe::utils::readSourceFileLines( const std::string& filename)
{
	std::vector<std::string> rt;
	readSourceFileLines_( filename, rt);
	return rt;
}

std::string _Wolframe::utils::getFileType( const std::string& filename)
{
	enum
	{
		B10000000 = 128,
		B11000000 = 128 + 64,
		B11100000 = 128 + 64 + 32,
		B11110000 = 128 + 64 + 32 + 16,
		B11111000 = 128 + 64 + 32 + 16 + 8,
		B11111100 = 128 + 64 + 32 + 16 + 8 + 4,
		B11111110 = 128 + 64 + 32 + 16 + 8 + 4 + 2,
		B11111111 = 128 + 64 + 32 + 16 + 8 + 4 + 2 + 1
	};
	std::string source;
	readFileContent( filename, source);
	if (source[0] == '<') return "XML";
	std::string::const_iterator si = source.begin(), se = source.end();
	bool ascii = true;
	bool utf8 = true;
	for (; si != se; ++si)
	{
		if (!*si) return "";
		if (*si < 0) ascii = false;
		if ((*si & B11000000) != B11000000) utf8 = false;
		if ((*si & B11100000) == B11000000) for (int ii=1; ii>0 && si != se; --ii,++si) if ((*si & B11000000) != B10000000) utf8 = false;
		if ((*si & B11110000) == B11100000) for (int ii=2; ii>0 && si != se; --ii,++si) if ((*si & B11000000) != B10000000) utf8 = false;
		if ((*si & B11111000) == B11110000) for (int ii=3; ii>0 && si != se; --ii,++si) if ((*si & B11000000) != B10000000) utf8 = false;
		if ((*si & B11111100) == B11111000) for (int ii=4; ii>0 && si != se; --ii,++si) if ((*si & B11000000) != B10000000) utf8 = false;
		if ((*si & B11111110) == B11111100) for (int ii=5; ii>0 && si != se; --ii,++si) if ((*si & B11000000) != B10000000) utf8 = false;
		if ((*si & B11111111) == B11111110) for (int ii=6; ii>0 && si != se; --ii,++si) if ((*si & B11000000) != B10000000) utf8 = false;
	}
	if (ascii) return "TEXT:ASCII";
	if (utf8) return "TEXT:UTF-8";
	return "TEXT";
}

boost::property_tree::ptree _Wolframe::utils::readPropertyTreeFile( const std::string& filename)
{
	std::string filetype = getFileType( filename);
	if (filetype.empty()) throw std::runtime_error( "Configuration file is not recognized as TEXT or XML");
	boost::property_tree::ptree rt;

	if (boost::istarts_with( filetype, "XML"))
	{
		namespace opt = boost::property_tree::xml_parser;
		read_xml( filename, rt, opt::no_comments | opt::trim_whitespace);
	}
	else if (boost::istarts_with( filetype, "TEXT"))
	{
		read_info( filename, rt);
	}
	else
	{
		throw std::runtime_error( std::string( "file type not recognized for '") + filename + "'");
	}
	return rt;
}


