/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file types/bignum.hpp
///\brief Defines a bignum type for the DDLs used for forms
#ifndef _Wolframe_TYPES_BIGNUM_HPP_INCLUDED
#define _Wolframe_TYPES_BIGNUM_HPP_INCLUDED
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace _Wolframe {
namespace types {

struct Bignum
{
private:
	static bool enter();
	static void leave();

public:
	Bignum( const std::string& v)
		:m_value(v){}
	~Bignum() {}

	Bignum& operator+( const Bignum& a);
	Bignum& operator-( const Bignum& a);
	Bignum& operator*( const Bignum& a);
	Bignum& operator/( const Bignum& a);

	template <typename Type>
	bool set( const Type& o)
	{
		try
		{
			m_value = boost::lexical_cast<std::string>(o);
			std::string::const_iterator itr = m_value.begin();
			if (*itr == '-') ++itr;
			for (; itr!=m_value.end(); ++itr)
			{
				if (*itr < '0' || *itr > '9')
				{
					m_value.clear();
					throw std::logic_error("cannot convert to bigint");
				}
			}
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}

	template <typename Type>
	bool get( Type& o)
	{
		try
		{
			o = boost::lexical_cast<Type>( m_value);
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}
private:
	std::string m_value;
};

}} //namespace
#endif
