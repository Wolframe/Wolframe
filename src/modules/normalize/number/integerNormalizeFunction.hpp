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
///\file integerNormalizeFunction.hpp
///\brief Normalizer for signed or unsigned integer numbers
#ifndef _LANGBIND_INTEGER_NORMALIZE_FUNCTION_HPP_INCLUDED
#define _LANGBIND_INTEGER_NORMALIZE_FUNCTION_HPP_INCLUDED
#include "types/normalizeFunction.hpp"
#include "types/variant.hpp"
#include <string>

namespace _Wolframe {
namespace langbind {

class IntegerNormalizeFunction :public types::NormalizeFunction
{
public:
	IntegerNormalizeFunction( std::size_t size_,
				  types::Variant::Data::UInt max_)
		:m_max(max_)
		,m_size(size_)
		,m_sign(true){}

	IntegerNormalizeFunction( const std::vector<types::Variant>& arg);
	IntegerNormalizeFunction( const IntegerNormalizeFunction& o)
		:m_max(o.m_max)
		,m_size(o.m_size)
		,m_sign(o.m_sign)
		{}

	virtual types::Variant execute( const types::Variant& inp) const;
	virtual const char* name() const {return m_sign?"integer":"unsigned";}
	virtual types::NormalizeFunction* copy() const {return new IntegerNormalizeFunction(*this);}

protected:
	types::Variant::Data::UInt m_max;
	std::size_t m_size;
	bool m_sign;
};

class UnsignedNormalizeFunction
	:public IntegerNormalizeFunction
{
public:
	UnsignedNormalizeFunction( std::size_t size_,
				  types::Variant::Data::UInt max_)
		:IntegerNormalizeFunction(size_,max_)
		{m_sign = false;}
	UnsignedNormalizeFunction( const std::vector<types::Variant>& arg);
	UnsignedNormalizeFunction( const UnsignedNormalizeFunction& o)
		:IntegerNormalizeFunction(o){}
};
}}
#endif

