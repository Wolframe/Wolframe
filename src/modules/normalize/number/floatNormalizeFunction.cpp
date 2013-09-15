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
///\file modules/normalize/number/floatNormalizeFunction.cpp
#include "floatNormalizeFunction.hpp"
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace langbind;

types::Variant FloatNormalizeFunction::execute( const types::Variant& inp) const
{
	if (inp.type() == types::Variant::Int)
	{
		types::Variant::Data::Int val = inp.toint();
		if (val < 0)
		{
			if ((double)-val > m_max) throw std::runtime_error( "number out of range");
		}
		else
		{
			if ((double)val > m_max) throw std::runtime_error( "number out of range");
		}
		return inp;
	}
	if (inp.type() == types::Variant::UInt)
	{
		types::Variant::Data::UInt val = inp.touint();
		if ((double)val > m_max) throw std::runtime_error( "number out of range");
		return inp;
	}
	if (inp.type() == types::Variant::Double)
	{
		double val = inp.todouble();
		if (val > m_max) throw std::runtime_error( "number out of range");
		return inp;
	}
	if (inp.type() != types::Variant::String)
	{
		double val = inp.todouble();
		if (val > m_max) throw std::runtime_error( "number out of range");
		return val;
	}
	std::string str( inp.tostring());

	std::string::const_iterator ii = str.begin(), ee = str.end();
	std::size_t cntG = m_sizeG?m_sizeG:std::numeric_limits<std::size_t>::max();
	std::size_t cntF = m_sizeF?m_sizeF:std::numeric_limits<std::size_t>::max();

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
	if (ii != ee)
	{
		if (*ii >= '0' && *ii <= '9') throw std::runtime_error( "number out of range");
		throw std::runtime_error( std::string("illegal token '") + *ii + "' in number");
	}
	try
	{
		return types::Variant( boost::lexical_cast<double>( str));
	}
	catch (...)
	{
	}
	return str;
}

