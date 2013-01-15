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
#include <stdexcept>
#include <iostream>

namespace _Wolframe {
namespace types {

class Bignum
{
public:
	Bignum()
		:m_value("0"),m_dec(0){}
	Bignum( const std::string& v, std::size_t p=0)
		:m_value(v),m_dec(p){}
	Bignum( const Bignum& o)
		:m_value(o.m_value),m_dec(o.m_dec){}
	~Bignum(){}

#if WITH_LIBGMP
	Bignum& operator+=( const Bignum& a);
	Bignum& operator-=( const Bignum& a);
	Bignum& operator*=( const Bignum& a);
	Bignum& operator/=( const Bignum& a);

	Bignum operator+( const Bignum& a) const		{Bignum rt(*this); return rt += a;}
	Bignum operator-( const Bignum& a) const		{Bignum rt(*this); return rt -= a;}
	Bignum operator*( const Bignum& a) const		{Bignum rt(*this); return rt *= a;}
	Bignum operator/( const Bignum& a) const		{Bignum rt(*this); return rt /= a;}
	Bignum pow( unsigned int a) const;
#endif
	Bignum operator-() const;
	Bignum& neg();

	bool set( const std::string& val);
	void get( std::string& val) const;

	std::size_t size() const				{return m_value.size();}


private:
	std::string m_value;			//< number digits with sign
	std::size_t m_dec;			//< number of decimal places (in m_value)
};

}} //namespace

std::ostream& operator<<( std::ostream& o, const _Wolframe::types::Bignum& b );

#endif
