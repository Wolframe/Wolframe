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

using namespace _Wolframe;
using namespace langbind;

types::Variant IntegerNormalizeFunction::execute( const types::Variant& inp) const
{
	if (inp.type() == types::Variant::int_) return inp;
	if (inp.type() == types::Variant::uint_) return inp;
	if (inp.type() == types::Variant::bool_) return inp.toint();
	if (inp.type() == types::Variant::double_) return inp.toint();
	std::string str( inp.tostring());

	if (!m_size) return str;
	std::string::const_iterator ii = str.begin(), ee = str.end();
	std::size_t cnt = m_size;
	if (m_sign)
	{
		if (ii != ee && *ii == '-') ++ii;
	}
	for (; cnt && ii != ee && *ii >= '0' && *ii <= '9'; ++ii, --cnt);
	if (ii != ee)
	{
		if (cnt) std::runtime_error( "number out of range");
		std::runtime_error( std::string("illegal token '") + *ii + "' in number");
	}
	return str;
}

