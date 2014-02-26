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
///\file fixedpointNormalizeFunction.hpp
///\brief Normalizer for fixedpoint point numbers
#ifndef _LANGBIND_FIXEDPOINT_NORMALIZE_FUNCTION_HPP_INCLUDED
#define _LANGBIND_FIXEDPOINT_NORMALIZE_FUNCTION_HPP_INCLUDED
#include "types/normalizeFunction.hpp"
#include "types/variant.hpp"
#include <string>
#include <limits>

namespace _Wolframe {
namespace langbind {

class FixedpointNormalizeFunction :public types::NormalizeFunction
{
public:
	FixedpointNormalizeFunction( std::size_t sizeG_, std::size_t sizeF_)
		:m_sizeG(sizeG_)
		,m_sizeF(sizeF_)
	{
		if (sizeG_ + sizeF_ > 15) throw std::runtime_error( "fixed point number specification out of range (maximum 15 digits)");
		m_epsilon = 0.1;
		m_base = 1.0;
		m_max = 1.0;
		std::size_t ii;
		for (ii=0; ii<m_sizeF; ++ii) m_base *= 10;
		for (ii=0; ii<m_sizeG; ++ii) m_max *= 10;
		m_epsilon /= m_base;
		m_epsilon += std::numeric_limits<double>::epsilon();
	}
	FixedpointNormalizeFunction( const FixedpointNormalizeFunction& o)
		:m_sizeG(o.m_sizeG)
		,m_sizeF(o.m_sizeF)
		,m_base(o.m_base)
		,m_epsilon(o.m_epsilon)
		,m_max(o.m_max){}

	virtual types::Variant execute( const types::Variant& inp) const;
	virtual const char* name() const {return "fixedpoint";}
	virtual types::NormalizeFunction* copy() const {return new FixedpointNormalizeFunction(*this);}

private:
	std::size_t m_sizeG;
	std::size_t m_sizeF;
	double m_base;
	double m_epsilon;
	double m_max;
};

}}
#endif

