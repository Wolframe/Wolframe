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
///\file integerNormalizeFunction.cpp
#include "integerNormalizeFunction.hpp"
#include <limits>

using namespace _Wolframe;
using namespace langbind;

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

IntegerNormalizeFunction::IntegerNormalizeFunction( const std::vector<types::Variant>& arg)
{
	m_sign = true;
	getIntegerDimArgument( arg, m_size, m_max);
}

UnsignedNormalizeFunction::UnsignedNormalizeFunction( const std::vector<types::Variant>& arg)
	:IntegerNormalizeFunction( arg)
{
	m_sign = true;
}

types::Variant IntegerNormalizeFunction::execute( const types::Variant& inp) const
{
	typedef types::Variant::Data::UInt UInt;
	typedef types::Variant::Data::Int Int;
	bool do_convert = true;
	bool isSigned = false;

	if (!inp.defined())
	{
		if (m_sign)
		{
			return types::Variant( (Int)0);
		}
		else
		{
			return types::Variant( (UInt)0);
		}
	}
	if (inp.type() == types::Variant::Int)
	{
		Int val = inp.toint();
		if (val < 0)
		{
			if ((UInt)-val > m_max) throw std::runtime_error( "number out of range");
		}
		else
		{
			if ((UInt)val > m_max) throw std::runtime_error( "number out of range");
		}
		return inp;
	}
	if (inp.type() == types::Variant::UInt)
	{
		UInt val = inp.touint();
		if (val > m_max) throw std::runtime_error( "number out of range");
		return inp;
	}
	if (inp.type() == types::Variant::Bool)
	{
		UInt val = inp.touint();
		if (val > m_max) throw std::runtime_error( "number out of range");
		return inp;
	}
	if (inp.type() == types::Variant::Double)
	{
		Int val = inp.toint();
		if (val < 0)
		{
			if ((UInt)-val >= m_max) throw std::runtime_error( "number out of range");
		}
		else
		{
			if ((UInt)val >= m_max) throw std::runtime_error( "number out of range");
		}
		return inp;
	}
	if (inp.type() != types::Variant::String && inp.type() != types::Variant::BigNumber)
	{
		UInt val = inp.touint();
		if (val >= m_max) throw std::runtime_error( "number out of range");
		return val;
	}
	std::string str( inp.tostring());

	UInt val = 0, pval = 0;
	std::string::const_iterator ii = str.begin(), ee = str.end();
	if (m_sign)
	{
		if (ii != ee && *ii == '-')
		{
			++ii;
			isSigned = true;
		}
	}
	for (; ii != ee && *ii >= '0' && *ii <= '9'; ++ii)
	{
		pval = val;
		val = val * 10 + (*ii - '0');
		if (pval > val) do_convert = false;
	}
	if (ii != ee)
	{
		throw std::runtime_error( std::string("illegal token '") + *ii + "' in number");
	}
	if (do_convert)
	{
		if (isSigned)
		{
			if (val > (UInt)std::numeric_limits<Int>::max())
			{
				return str;
			}
			else
			{
				return types::Variant( (Int)( val));
			}
		}
		else
		{
			return types::Variant( val);
		}
	}
	return str;
}

