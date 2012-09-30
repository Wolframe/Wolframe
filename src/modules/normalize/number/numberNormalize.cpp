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
///\file modules/normalize/number/numberNormalize.cpp
///\brief Implementation of normalization functions for simple ASCII numbers
#include "numberNormalize.hpp"
#include "trimNormalizeFunction.hpp"
#include "integerNormalizeFunction.hpp"
#include "floatNormalizeFunction.hpp"
#include "utils/miscUtils.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <limits>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static utils::CharTable opTab( "(,)");
static utils::CharTable numTab( "0123456789");

static std::size_t parseNumber( std::string::const_iterator& ii, const std::string::const_iterator ee)
{
	std::string tok;
	char ch = utils::parseNextToken( tok, ii, ee, opTab, numTab);
	if (ch == '\0') throw std::runtime_error( "unexpected end of dimension description");
	if (ch == ',' || ch == ')' || ch == '(') throw std::runtime_error( "syntax error in dimension description");
	return boost::lexical_cast<std::size_t>( tok);
}

static std::pair<std::size_t,std::size_t> parseDimension2Description( std::string::const_iterator& ii, const std::string::const_iterator ee)
{
	std::string tok;
	std::size_t gsize=0,fsize=0;
	char ch;
	ch = utils::parseNextToken( tok, ii, ee, opTab, numTab);
	if (ch == '\0') return std::pair<std::size_t,std::size_t>( 0, 0);
	if (ch != '(') throw std::runtime_error( "syntax error in dimension description");
	gsize = parseNumber( ii, ee);
	ch = utils::parseNextToken( tok, ii, ee, opTab, numTab);
	if (ch == ')') return std::pair<std::size_t,std::size_t>( 0, gsize);
	if (ch == '\0') throw std::runtime_error( "unexpected end in dimension description");
	if (ch != ',') throw std::runtime_error( "syntax error in dimension description");
	fsize = parseNumber( ii, ee);
	ch = utils::parseNextToken( tok, ii, ee, opTab, numTab);
	if (ch == '\0') throw std::runtime_error( "unexpected end in dimension description");
	if (ch != ')') throw std::runtime_error( "expected ')' at end of dimension description");
	return std::pair<std::size_t,std::size_t>( gsize, fsize);
}

static std::size_t parseDimension1Description( std::string::const_iterator& ii, const std::string::const_iterator ee)
{
	std::string tok;
	std::size_t size=0;
	char ch;
	ch = utils::parseNextToken( tok, ii, ee, opTab, numTab);
	if (ch == '\0') return 0;
	if (ch != '(') throw std::runtime_error( "syntax error in dimension description");
	size = parseNumber( ii, ee);
	ch = utils::parseNextToken( tok, ii, ee, opTab, numTab);
	if (ch != ')') throw std::runtime_error( "syntax error in dimension description");
	return size;
}

static std::pair<std::size_t,std::size_t> parseFloatDescription( std::string::const_iterator& ii, const std::string::const_iterator ee)
{
	std::pair<std::size_t,std::size_t> rt = parseDimension2Description( ii, ee);
	if (utils::gotoNextToken( ii, ee))
	{
		throw std::runtime_error( "unexpected token after dimension description");
	}
	return rt;
}

static std::size_t parseIntegerDescription( std::string::const_iterator& ii, const std::string::const_iterator ee)
{
	std::size_t rt = parseDimension1Description( ii, ee);
	if (utils::gotoNextToken( ii, ee))
	{
		throw std::runtime_error( "unexpected token after dimension description");
	}
	return rt;
}

NormalizeFunction* _Wolframe::langbind::createNumberNormalizeFunction( ResourceHandle&, const std::string& description)
{
	try
	{
		bool doTrim = false;
		std::string type;
		std::string::const_iterator ii = description.begin(), ee = description.end();
		char ch = utils::parseNextToken( type, ii, ee, opTab);
		if (opTab[ch]) throw std::runtime_error( "type identifier expected");
		if (boost::algorithm::iequals( type, "trim"))
		{
			doTrim = true;
			ch = utils::parseNextToken( type, ii, ee, opTab);
			if (ch == '\0') return new TrimNormalizeFunction();
			if (opTab[ch]) throw std::runtime_error( "type identifier expected");
		}
		if (boost::algorithm::iequals( type, "integer"))
		{
			std::size_t dim = parseIntegerDescription( ii, ee);
			return new IntegerNormalizeFunction( true, dim, doTrim);
		}
		else if (boost::algorithm::iequals( type, "unsigned"))
		{
			std::size_t dim = parseIntegerDescription( ii, ee);
			return new IntegerNormalizeFunction( false, dim, doTrim);
		}
		else if (boost::algorithm::iequals( type, "float"))
		{
			std::pair<std::size_t,std::size_t> dim = parseFloatDescription( ii, ee);
			return new FloatNormalizeFunction( dim.first, dim.second, doTrim);
		}
		else
		{
			throw std::runtime_error( std::string( "unknown number type '") + type + "'");
		}
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string( "error in number normalize function description: ") + e.what());
	}
}


