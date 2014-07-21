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
///\file floatNormalizeFunction.hpp
///\brief Normalizer for floating point numbers
#ifndef _LANGBIND_FLOAT_NORMALIZE_FUNCTION_HPP_INCLUDED
#define _LANGBIND_FLOAT_NORMALIZE_FUNCTION_HPP_INCLUDED
#include "types/normalizeFunction.hpp"
#include "types/variant.hpp"
#include <string>

namespace _Wolframe {
namespace langbind {

class FloatNormalizeFunction :public types::NormalizeFunction
{
public:
	FloatNormalizeFunction( const std::vector<types::Variant>& arg);
	FloatNormalizeFunction( const FloatNormalizeFunction& o)
		:m_sizeG(o.m_sizeG)
		,m_sizeF(o.m_sizeF)
		,m_max(o.m_max){}

	virtual types::Variant execute( const types::Variant& inp) const;
	virtual const char* name() const {return "float";}
	virtual types::NormalizeFunction* copy() const {return new FloatNormalizeFunction(*this);}

private:
	std::size_t m_sizeG;
	std::size_t m_sizeF;
	double m_max;
};

}}
#endif

