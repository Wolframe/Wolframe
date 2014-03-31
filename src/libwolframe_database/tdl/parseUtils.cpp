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
///\brief Implementation of utility functions for parsing
///\file tdl/parseUtils.cpp
#include "tdl/parseUtils.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::tdl;

static const utils::CharTable g_optab( ";:-,.=)(<>[]{}/&%*|+-#?!$");

bool tdl::isIdentifier( const std::string& str)
{
	std::string::const_iterator si = str.begin(), se = str.end();
	if (si == se) return false;
	if (!isAlpha(*si)) return false;
	for (++si; si != se && isAlphaNumeric(*si); ++si){}
	return (si == se);
}

std::string tdl::errorTokenString( char ch, const std::string& tok)
{
	if (g_optab[ch])
	{
		std::string rt;
		rt.push_back( ch);
		return rt;
	}
	if (ch == '"' || ch == '\'')
	{
		std::string rt( "string ");
		rt.push_back( ch);
		rt.append( tok);
		rt.push_back( ch);
		return rt;
	}
	if (ch == 0)
	{
		return "end of file";
	}
	return tok;
}

char tdl::gotoNextToken( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator se)
{
	const char* commentopr = langdescr->eoln_commentopr();
	char ch;
	while ((ch = utils::gotoNextToken( si, se)) != 0)
	{
		if (ch == commentopr[0])
		{
			std::string::const_iterator ti = si+1;
			std::size_t ci = 1;
			while (commentopr[ci] && ti != se && commentopr[ci] == *ti)
			{
				++ci;
				++ti;
			}
			if (!commentopr[ci])
			{
				// skip to end of line
				while (ti != se && *ti != '\n') ++ti;
				si = ti;
				continue;
			}
		}
		break;
	}
	return ch;
}

char tdl::parseNextToken( const LanguageDescription* langdescr, std::string& tok, std::string::const_iterator& si, std::string::const_iterator se)
{
	char ch = gotoNextToken( langdescr, si, se);
	if (!ch) return 0;
	return utils::parseNextToken( tok, si, se, g_optab);
}

std::vector<std::string> tdl::parse_INTO_path( const LanguageDescription* langdescr, std::string::const_iterator& si, std::string::const_iterator se)
{
	std::vector<std::string> rt;
	for (;;)
	{
		std::string output;
		char ch = parseNextToken( langdescr, output, si, se);
		if (!ch) throw std::runtime_error( "unexpected end of description. result tag path expected after INTO");
		if (ch == '.' && output.empty()) output.push_back(ch);

		if (output.empty())
		{
			throw std::runtime_error( "identifier or '.' expected after INTO");
		}
		rt.push_back( output);
		ch = gotoNextToken( langdescr, si, se);
		if (ch != '/') break;
		++si;
	}
	return rt;
}

std::vector<std::string> tdl::parseTemplateArguments( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	std::vector<std::string> rt;
	std::string tok;
	if (gotoNextToken( langdescr, si, se) == '<')
	{
		++si;
		for (;;)
		{
			char ch = parseNextToken( langdescr, tok, si, se);
			if (!ch) throw std::runtime_error( "unexpected end of template argument list");
			if (ch == ',') std::runtime_error( "expected template argument identifier before comma ','");
			if (ch == '>') break;
			if (!isAlpha(ch)) throw std::runtime_error( std::string( "template argument is not an identifier: '") + errorTokenString( ch, tok) + "'");
			rt.push_back( tok);
			ch = parseNextToken( langdescr, tok, si, se);
			if (ch == '>') break;
			if (ch == ',') continue;
			if (ch == ',') std::runtime_error( "expected comma ',' or end of template identifier list '>'");
		}
	}
	return rt;
}

void tdl::checkUniqOccurrence( int id, unsigned int& mask, const utils::IdentifierTable& idtab)
{
	unsigned int idshft = (1 << (id+1));
	if (0 != (mask & idshft))
	{
		throw std::runtime_error( std::string( idtab.idstring(id)) + " specified twice in a command");
	}
	mask |= idshft;
}
