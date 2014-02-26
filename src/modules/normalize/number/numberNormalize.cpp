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

static void getIntegerDimArgument( const std::vector<types::Variant>& arg, std::size_t& dim, types::Variant::Data::UInt& maxval)
{
	if (arg.size() > 1) throw std::runtime_error("too many arguments for integer normalize function");
	dim = std::numeric_limits<std::size_t>::max();
	if (arg.size())
	{
		types::Variant::Data::UInt vdim = arg.at(0).touint();
		if (vdim >= std::numeric_limits<std::size_t>::max())
		{
			throw std::runtime_error( "parameter out of range for integer normalize function");
		}
		dim = (std::size_t)vdim;
		maxval = getMax( dim)-1;
	}
	else
	{
		dim = std::numeric_limits<std::size_t>::max();
		maxval = std::numeric_limits<types::Variant::Data::UInt>::max();
	}
}

types::NormalizeFunction* _Wolframe::langbind::createIntegerNormalizeFunction( types::NormalizeResourceHandle*, const std::vector<types::Variant>& arg)
{
	std::size_t dim;
	types::Variant::Data::UInt maxval;
	getIntegerDimArgument( arg, dim, maxval);
	return new IntegerNormalizeFunction( true, dim, maxval);
}

types::NormalizeFunction* _Wolframe::langbind::createUnsignedNormalizeFunction( types::NormalizeResourceHandle*, const std::vector<types::Variant>& arg)
{
	std::size_t dim;
	types::Variant::Data::UInt maxval;
	getIntegerDimArgument( arg, dim, maxval);
	return new IntegerNormalizeFunction( false, dim, maxval);
}

static void getFractionDimArgument( const std::vector<types::Variant>& arg, std::size_t& sizeG, std::size_t& sizeF)
{
	if (arg.size() > 2) throw std::runtime_error("too many arguments for float/fixedpoint normalize function");
	sizeG = 0;
	sizeF = std::numeric_limits<std::size_t>::max();
	if (arg.size() == 1)
	{
		types::Variant::Data::UInt vsizeF = arg.at(0).touint();
		if (vsizeF >= std::numeric_limits<std::size_t>::max())
		{
			throw std::runtime_error( "parameter out of range for float/fixedpoint normalize function");
		}
		sizeF = (std::size_t)vsizeF;
	}
	else if (arg.size() == 2)
	{
		types::Variant::Data::UInt vsizeG = arg.at(0).touint();
		types::Variant::Data::UInt vsizeF = arg.at(1).touint();
		if (vsizeF >= std::numeric_limits<std::size_t>::max() || vsizeG >= std::numeric_limits<std::size_t>::max())
		{
			throw std::runtime_error( "parameter out of range for float/fixedpoint normalize function");
		}
		sizeG = (std::size_t)vsizeG;
		sizeF = (std::size_t)vsizeF;
	}
}

types::NormalizeFunction* _Wolframe::langbind::createFloatNormalizeFunction( types::NormalizeResourceHandle*, const std::vector<types::Variant>& arg)
{
	std::size_t sizeG;
	std::size_t sizeF;
	getFractionDimArgument( arg, sizeG, sizeF);
	double maxval;
	if (sizeG)
	{
		maxval = (double)getMax( sizeG);
	}
	else
	{
		maxval = std::numeric_limits<double>::max();
	}
	return new FloatNormalizeFunction( sizeG, sizeF, maxval);
}

types::NormalizeFunction* _Wolframe::langbind::createFixedpointNormalizeFunction( types::NormalizeResourceHandle*, const std::vector<types::Variant>& arg)
{
	std::size_t sizeG = 12;
	std::size_t sizeF = 3;
	if (arg.size())
	{
		getFractionDimArgument( arg, sizeG, sizeF);
	}
	return new FixedpointNormalizeFunction( sizeG, sizeF);
}

