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

types::Variant IntegerNormalizeFunction::execute( const types::Variant& inp) const
{
	typedef types::Variant::Data::UInt UInt;
	typedef types::Variant::Data::Int Int;
	bool do_convert = true;
	bool isSigned = false;

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

