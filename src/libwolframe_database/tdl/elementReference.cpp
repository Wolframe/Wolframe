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
///\brief Implementation of parsing result methods for atomic element references in subroutine calls or embedded database statements
///\file tdl/elementReference.cpp
#include "tdl/elementReference.hpp"
#include "tdl/parseUtils.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::tdl;

ElementReference ElementReference::parseEmbeddedReference( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	char ch = utils::gotoNextToken( si, se);
	if (ch == '(')
	{
		char sb,eb;
		sb = '('; eb = ')';
		++si;
		std::string::const_iterator argstart = si;
		std::string tok;
		ch = parseNextToken( langdescr, tok, si, se);
		for (; ch && ch != sb && ch != eb; ch=parseNextToken( langdescr, tok, si, se));
		if (ch == eb)
		{
			std::string selector = boost::algorithm::trim_copy( std::string( argstart, si-1));
			return ElementReference( ElementReference::SelectorPath, selector);
		}
		else
		{
			throw std::runtime_error( std::string( "missing close bracket for expression") + ((eb==')')?"')'":"']'"));
		}
	}
	else if (ch == '#')
	{
		// Internal LOOPCOUNT variable:
		++si;
		return ElementReference( ElementReference::LoopCounter);
	}
	else if (isAlphaNumeric(ch))
	{
		// Parse namespace:
		std::string namspace;
		if (isAlpha(ch))
		{
			std::string::const_iterator argstart = si;
			for (; si!=se && isAlphaNumeric(*si); ++si);
			if (*si == '.')
			{
				namspace.append( argstart, si);
				++si;
				ch = (si==se)?0:*si;
			}
			else
			{
				si = argstart;
			}
		}
		// Parse set reference:
		if (isDigit(ch))
		{
			std::string::const_iterator argstart = si;
			for (; si!=se && isDigit(*si); ++si);
			if (si != se && isAlphaNumeric(*si))
			{
				throw std::runtime_error( "operator or space expected after numeric reference");
			}
			return ElementReference( ElementReference::IndexSetElement, namspace, boost::lexical_cast<std::size_t>( std::string( argstart, si)));
		}
		else if (isAlpha(ch))
		{
			std::string::const_iterator argstart = si;
			for (; si!=se && isAlphaNumeric(*si); ++si);
			return ElementReference( ElementReference::NamedSetElement, namspace, std::string( argstart, si));
		}
		else
		{
			throw std::runtime_error( "expected open bracket '(' or alphanumeric result reference");
		}
	}
	else
	{
		throw std::runtime_error( "expected open bracket '(' or alphanumeric result reference");
	}
}

ElementReference ElementReference::parsePlainReference( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	std::string pp;
	std::string tok;
	char ch = gotoNextToken( langdescr, si, se);
	if (ch == '"' || ch == '\'' || isDigit( ch))
	{
		(void)parseNextToken( langdescr, tok, si, se);
		return ElementReference( ElementReference::Constant, tok);
	}
	else if (ch == '$')
	{
		return parseEmbeddedReference( langdescr, si, se);
	}
	else
	{
		tok = parseSelectorPath( langdescr, si, se);
		return ElementReference( ElementReference::SelectorPath, tok);
	}
}


