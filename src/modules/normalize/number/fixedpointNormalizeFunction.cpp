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
///\file fixedpointNormalizeFunction.cpp
#include "fixedpointNormalizeFunction.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>
#include <cmath>

using namespace _Wolframe;
using namespace langbind;

types::Variant FixedpointNormalizeFunction::execute( const types::Variant& inp) const
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
		return val;
	}
	if (inp.type() == types::Variant::UInt)
	{
		types::Variant::Data::UInt val = inp.touint();
		if ((double)val > m_max) throw std::runtime_error( "number out of range");
		return val;
	}
	if (inp.type() == types::Variant::Double)
	{
		double val = inp.todouble();
		double fval = std::floor( (val + m_epsilon) * m_base);
		boost::uint64_t xx;
		std::string rt;
		if (val < 0.0)
		{
			if (val < -m_max) throw std::runtime_error( "number out of range");
			rt.push_back('_');
			xx = boost::numeric_cast<boost::uint64_t>( -fval);
		}
		else
		{
			if (val > m_max) throw std::runtime_error( "number out of range");
			xx = boost::numeric_cast<boost::uint64_t>( fval);
		}
		std::string sval = boost::lexical_cast<std::string>( xx);
		if (sval.size() <= m_sizeF)
		{
			rt.append( "0.");
			rt.append( m_sizeF - sval.size(), '0');
			rt.append( sval);
		}
		else
		{
			rt.append( sval.c_str(), sval.size() - m_sizeF);
			rt.append( ".");
			rt.append( sval.c_str() + sval.size() - m_sizeF);
		}
		return rt;
	}
	if (inp.type() != types::Variant::String)
	{
		types::Variant::Data::Int val = inp.toint();
		if ((double)val > m_max) throw std::runtime_error( "number out of range");
		return val;
	}
	std::string str( inp.tostring());
	std::string::const_iterator ii = str.begin(), ee = str.end();
	std::size_t gg = 0;
	std::size_t ff = 0;

	if (ii != ee && *ii == '-') ++ii;
	for (; ii != ee && *ii >= '0' && *ii <= '9'; ++ii,++gg);
	if (ii != ee)
	{
		if (*ii == '.')
		{
			++ii;
			for (; ii != ee && *ii >= '0' && *ii <= '9'; ++ii,++ff);
		}
	}
	if (ii != ee)
	{
		if (*ii >= '0' && *ii <= '9') throw std::runtime_error( "number out of range");
		throw std::runtime_error( std::string("illegal token '") + *ii + "' in number");
	}
	if (m_sizeG < gg) throw std::runtime_error( "number out of range");
	if (m_sizeF < ff) throw std::runtime_error( "number out of range");
	return inp;
}

