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
#include "utils/miscUtils.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <limits>

using namespace _Wolframe;
using namespace langbind;

namespace
{

class TrimNormalizeFunction :public NormalizeFunction
{
public:
	TrimNormalizeFunction(){}
	virtual std::string execute( const std::string& str) const
	{
		std::string::const_iterator ii = str.begin(), ee = str.end();
		while (ii != ee && *ii <= 32 && *ii >= 0) ++ii;
		std::string::const_iterator ti = ii, te = ii;
		for (; ii != ee; ++ii)
		{
			if ((unsigned char)*ii > 32) te = ii+1;
		}
		if (ti == str.begin() && te == str.end())
		{
			return str;
		}
		else
		{
			return std::string( ti, te);
		}
	}
};

class IntegerNormalizeFunction :public NormalizeFunction
{
public:
	IntegerNormalizeFunction( bool sign_, std::size_t size_, bool trim_)
		:m_size(size_)
		,m_sign(sign_)
		,m_trim(trim_){}

	virtual std::string execute( const std::string& str) const
	{
		if (!m_size) return str;
		std::string::const_iterator ii = str.begin(), ee = str.end();
		std::size_t cnt = m_size;
		if (m_trim) while (ii != ee && *ii <= 32 && *ii >= 0) ++ii;
		std::string::const_iterator ti = ii;
		if (m_sign)
		{
			if (ii != ee && *ii == '-') ++ii;
		}
		for (; cnt && ii != ee && *ii >= '0' && *ii <= '9'; ++ii, --cnt);
		std::string::const_iterator te = ii;
		if (m_trim) while (ii != ee && *ii <= 32 && *ii >= 0) ++ii;
		if (ii != ee)
		{
			if (cnt) std::runtime_error( "number out of range");
			std::runtime_error( std::string("illegal token '") + *ii + "' in number");
		}
		if (m_trim && (ti != str.begin() || te != str.end()))
		{
			return std::string( ti, te);
		}
		else
		{
			return str;
		}
	}
private:
	std::size_t m_size;
	bool m_sign;
	bool m_trim;
};

class FloatNormalizeFunction :public NormalizeFunction
{
public:
	FloatNormalizeFunction( std::size_t sizeG_, std::size_t sizeF_, bool trim_)
		:m_sizeG(sizeG_)
		,m_sizeF(sizeF_)
		,m_trim(trim_){}

	virtual std::string execute( const std::string& str) const
	{
		std::string::const_iterator ii = str.begin(), ee = str.end();
		std::size_t cntG = m_sizeG?m_sizeG:std::numeric_limits<std::size_t>::max();
		std::size_t cntF = m_sizeF?m_sizeF:std::numeric_limits<std::size_t>::max();

		if (m_trim) while (ii != ee && *ii <= 32 && *ii >= 0) ++ii;
		std::string::const_iterator ti = ii;
		if (ii != ee && *ii == '-') ++ii;
		for (; cntG && ii != ee && *ii >= '0' && *ii <= '9'; ++ii, --cntG);
		if (ii != ee)
		{
			if (*ii == '.')
			{
				++ii;
				for (; cntF && ii != ee && *ii >= '0' && *ii <= '9'; ++ii, --cntF);
			}
		}
		std::string::const_iterator te = ii;
		if (m_trim) while (ii != ee && *ii <= 32 && *ii >= 0) ++ii;
		if (ii != ee)
		{
			if (*ii >= '0' && *ii <= '9') std::runtime_error( "number out of range");
			std::runtime_error( std::string("illegal token '") + *ii + "' in number");
		}
		if (m_trim && (ti != str.begin() || te != str.end()))
		{
			return std::string( ti, te);
		}
		else
		{
			return str;
		}
	}
private:
	std::size_t m_sizeG;
	std::size_t m_sizeF;
	bool m_trim;
};

} //anonymous namespace

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


