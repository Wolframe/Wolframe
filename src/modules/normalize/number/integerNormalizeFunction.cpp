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
///\file modules/normalize/number/integerNormalizeFunction.cpp
#include "integerNormalizeFunction.hpp"
#include <limits>

using namespace _Wolframe;
using namespace langbind;

types::Variant IntegerNormalizeFunction::execute( const types::Variant& inp) const
{
	typedef types::Variant::Data::UInt_ UInt_;
	typedef types::Variant::Data::Int_ Int_;
	bool do_convert = true;
	bool isSigned = false;

	if (inp.type() == types::Variant::int_)
	{
		Int_ val = inp.toint();
		if (val < 0)
		{
			if ((types::Variant::Data::UInt_)-val > m_max) throw std::runtime_error( "number out of range");
		}
		else
		{
			if ((types::Variant::Data::UInt_)val > m_max) throw std::runtime_error( "number out of range");
		}
		return inp;
	}
	if (inp.type() == types::Variant::uint_)
	{
		UInt_ val = inp.touint();
		if (val > m_max) throw std::runtime_error( "number out of range");
		return inp;
	}
	if (inp.type() == types::Variant::bool_)
	{
		UInt_ val = inp.touint();
		if (val > m_max) throw std::runtime_error( "number out of range");
		return inp;
	}
	if (inp.type() == types::Variant::double_)
	{
		Int_ val = inp.toint();
		if (val < 0)
		{
			if ((types::Variant::Data::UInt_)-val >= m_max) throw std::runtime_error( "number out of range");
		}
		else
		{
			if ((types::Variant::Data::UInt_)val >= m_max) throw std::runtime_error( "number out of range");
		}
		return inp;
	}
	if (inp.type() != types::Variant::string_)
	{
		UInt_ val = inp.touint();
		if (val >= m_max) throw std::runtime_error( "number out of range");
		return val;
	}
	std::string str( inp.tostring());

	UInt_ val = 0, pval = 0;
	std::string::const_iterator ii = str.begin(), ee = str.end();
	std::size_t cnt = m_size;
	if (m_sign)
	{
		if (ii != ee && *ii == '-')
		{
			++ii;
			isSigned = true;
		}
	}
	for (; cnt && ii != ee && *ii >= '0' && *ii <= '9'; ++ii, --cnt)
	{
		pval = val;
		val = val * 10 + (*ii - '0');
		if (pval > val) do_convert = false;
	}
	if (ii != ee)
	{
		if (cnt) throw std::runtime_error( "number out of range");
		throw std::runtime_error( std::string("illegal token '") + *ii + "' in number");
	}
	if (do_convert)
	{
		if (isSigned)
		{
			if (val > (UInt_)std::numeric_limits<Int_>::max())
			{
				return str;
			}
			else
			{
				return types::Variant( (Int_)( val));
			}
		}
		else
		{
			return types::Variant( val);
		}
	}
	return str;
}

