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
///\file modules/normalize/number/numberNormalize.cpp
///\brief Implementation of normalization functions for simple ASCII numbers
#include "numberNormalize.hpp"
#include "integerNormalizeFunction.hpp"
#include "floatNormalizeFunction.hpp"
#include "fixedpointNormalizeFunction.hpp"
#include "types/variant.hpp"
#include "utils/parseUtils.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <limits>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static utils::CharTable opTab( ",");
static utils::CharTable numTab( "0123456789");

static std::size_t parseNumber( std::string::const_iterator& ii, const std::string::const_iterator ee)
{
	std::string tok;
	char ch = utils::parseNextToken( tok, ii, ee, opTab, numTab);
	if (ch == '\0') throw std::runtime_error( "unexpected end of dimension description");
	if (ch == ',') throw std::runtime_error( "syntax error in dimension description");
	return boost::lexical_cast<std::size_t>( tok);
}

static std::pair<std::size_t,std::size_t> parseDimension2Description( std::string::const_iterator& ii, const std::string::const_iterator ee)
{
	std::string tok;
	std::size_t gsize=0,fsize=0;
	char ch;
	gsize = parseNumber( ii, ee);
	ch = utils::parseNextToken( tok, ii, ee, opTab, numTab);
	if (ch == '\0') return std::pair<std::size_t,std::size_t>( 0, gsize);
	if (ch != ',') throw std::runtime_error( "syntax error in dimension description");
	fsize = parseNumber( ii, ee);
	if (utils::gotoNextToken( ii, ee)) throw std::runtime_error( "unexpected token at end of dimension description");
	return std::pair<std::size_t,std::size_t>( gsize, fsize);
}

static std::size_t parseDimension1Description( std::string::const_iterator& ii, const std::string::const_iterator ee)
{
	std::string tok;
	std::size_t size=0;
	size = parseNumber( ii, ee);
	if (utils::gotoNextToken( ii, ee)) throw std::runtime_error( "unexpected token at end of dimension description");
	return size;
}

static std::pair<std::size_t,std::size_t> parseFloatDescription( std::string::const_iterator& ii, const std::string::const_iterator ee)
{
	std::pair<std::size_t,std::size_t> rt = parseDimension2Description( ii, ee);
	return rt;
}

static std::size_t parseIntegerDescription( std::string::const_iterator& ii, const std::string::const_iterator ee)
{
	std::size_t rt = parseDimension1Description( ii, ee);
	return rt;
}

static types::Variant::Data::UInt getMax( std::size_t digits)
{
	typedef types::Variant::Data::UInt UInt;
	UInt mm = 1, pp = 1;
	std::size_t dd = 0;
	for (; dd < digits; ++dd)
	{
		mm *= 10;
		if (mm < pp) break;
		pp = mm;
	}
	if (dd == digits)
	{
		return mm;
	}
	else
	{
		return std::numeric_limits<UInt>::max();
	}
}

types::NormalizeFunction* _Wolframe::langbind::createNumberNormalizeFunction( ResourceHandle&, const std::string& name, const std::string& arg)
{
	try
	{
		std::string::const_iterator ii = arg.begin(), ee = arg.end();
		std::string type = boost::algorithm::to_lower_copy( name);
		if (boost::algorithm::iequals( type, "integer"))
		{
			typedef types::Variant::Data::UInt UInt;
			std::size_t dim;
			UInt maxval;

			if (utils::gotoNextToken( ii, ee))
			{
				dim = parseIntegerDescription( ii, ee);
				maxval = getMax( dim)-1;
			}
			else
			{
				dim = std::numeric_limits<std::size_t>::max();
				maxval = std::numeric_limits<UInt>::max();
			}
			return new IntegerNormalizeFunction( true, dim, maxval);
		}
		else if (boost::algorithm::iequals( type, "unsigned"))
		{
			typedef types::Variant::Data::UInt UInt;
			std::size_t dim;
			UInt maxval;

			if (utils::gotoNextToken( ii, ee))
			{
				dim = parseIntegerDescription( ii, ee);
				maxval = getMax( dim)-1;
			}
			else
			{
				dim = std::numeric_limits<std::size_t>::max();
				maxval = std::numeric_limits<UInt>::max();
			}
			return new IntegerNormalizeFunction( false, dim, maxval);
		}
		else if (boost::algorithm::iequals( type, "float"))
		{
			std::pair<std::size_t,std::size_t> dim;
			double maxval;

			if (utils::gotoNextToken( ii, ee))
			{
				dim = parseFloatDescription( ii, ee);
				maxval = (double)getMax( dim.first);
			}
			else
			{
				dim = std::pair<std::size_t,std::size_t>( std::numeric_limits<std::size_t>::max(), std::numeric_limits<std::size_t>::max());
				maxval = std::numeric_limits<double>::max();
			}
			return new FloatNormalizeFunction( dim.first, dim.second, maxval);
		}
		else if (boost::algorithm::iequals( type, "fixedpoint"))
		{
			std::pair<std::size_t,std::size_t> dim;

			if (utils::gotoNextToken( ii, ee))
			{
				dim = parseFloatDescription( ii, ee);
				return new FixedpointNormalizeFunction( dim.first, dim.second);
			}
			else
			{
				return new FixedpointNormalizeFunction( 12, 3);
			}
		}
		else
		{
			throw std::runtime_error( std::string( "unknown number type '") + name + "'");
		}
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string( "error in number normalize function description: ") + e.what());
	}
}


const std::vector<std::string>& _Wolframe::langbind::normalizeFunctions()
{
	struct NormalizeFunctions :public std::vector<std::string>
	{
		NormalizeFunctions()
		{
			push_back( "integer");
			push_back( "unsigned");
			push_back( "float");
			push_back( "fixedpoint");
		}
	};
	static NormalizeFunctions rt;
	return rt;
}

