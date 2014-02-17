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
//\file types/bignumber.hpp
//\brief Type for representing arbitrary precision fixed point numbers and big integers

#ifndef _Wolframe_TYPES_BIG_NUMBER_HPP_INCLUDED
#define _Wolframe_TYPES_BIG_NUMBER_HPP_INCLUDED
#include "types/integer.hpp"
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace types {

//\class BigNumber
//\brief Type for representing big numbers
struct BigNumber
{
	short scale() const			{return m_scale;}
	unsigned short size() const		{return m_size;}
	bool sign() const			{return m_sign;}
	const unsigned char* ar() const		{return m_ar;}

	static BigNumber* create( double val);
	static BigNumber* create( _WOLFRAME_INTEGER val);
	static BigNumber* create( _WOLFRAME_UINTEGER val);
	static BigNumber* create( const std::string& val);

	static void destroy( BigNumber* num);

	std::string tostring() const;
	_WOLFRAME_INTEGER toint() const;
	_WOLFRAME_UINTEGER touint() const;
	double todouble() const;

private:
	short m_scale;
	unsigned short m_size;
	bool m_sign;
	unsigned char m_ar[1];
};

//\class BigNumberR
//\brief Big BCD number reference
struct BigNumberR
	:public boost::shared_ptr<BigNumber>
{
	BigNumberR( BigNumber* ref)	:boost::shared_ptr<BigNumber>( ref, BigNumber::destroy){}
};

}}//namespace
#endif


