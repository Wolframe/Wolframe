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
///\file types/datetime.hpp
///\brief Date and time value type

#ifndef _Wolframe_TYPES_DATETIME_HPP_INCLUDED
#define _Wolframe_TYPES_DATETIME_HPP_INCLUDED
#include <cstring>
#include <boost/cstdint.hpp>

namespace _Wolframe {
namespace types {

//\brief Timestamp equivalent to a date time value for variant type
typedef boost::int64_t Timestamp;

//\class DateTime
//\brief Data type for normalized date time (absolute time without time zone info)
//\remark No date time arithmetics. Date time operations have to be implemented in a custom data type
class DateTime
{
public:
	DateTime( const DateTime& o)
		:m_impl(o.m_impl)
	{}

	DateTime( unsigned short YY,
		  unsigned short MM,
		  unsigned short DD,
		  unsigned short hh,
		  unsigned short mm,
		  unsigned short ss,
		  unsigned short ll=0,
		  unsigned short cc=0)
		:m_impl(YY,MM,DD,hh,mm,ss,ll,cc)
	{}

	DateTime( const Timestamp& t)
		:m_impl(t)
	{}

	Timestamp value() const
	{
		return m_impl.value();
	}

	void init( Timestamp value_)
	{
		m_impl.init( value_);
	}

private:
	struct Impl
	{
		unsigned int year:13;		//< [ 0..12]
		unsigned int month:4;		//< [13..16]
		unsigned int day:5;		//< [17..21]
		unsigned int hour:5;		//< [22..26]
		unsigned int minute:7;		//< [27..33]
		unsigned int second:7;		//< [34..40]
		unsigned int millisecond:10;	//< [41..50]
		unsigned int microsecond:10;	//< [51..60]

		Impl()
			:year(0),month(0),day(0),hour(0),minute(0),second(0),millisecond(0),microsecond(0)
		{}

		Impl( const Impl& o)
			:year(o.year),month(o.month),day(o.day),hour(o.hour),minute(o.minute),second(o.second),millisecond(o.millisecond),microsecond(o.microsecond)
		{}

		Impl( unsigned short YY,
			unsigned short MM,
			unsigned short DD,
			unsigned short hh,
			unsigned short mm,
			unsigned short ss,
			unsigned short ll=0,
			unsigned short cc=0)
			:year(YY),month(MM),day(DD),hour(hh),minute(mm),second(ss),millisecond(ll),microsecond(cc)
		{}

		Impl( const Timestamp& t)
		{
			init(t);
		}

		void init( Timestamp value_)
		{
			year = (value_ >>	(60-12)) & ((1<<12)-1);
			month = (value_ >>	(60-16)) & ((1<< 4)-1);
			day = (value_ >>	(60-21)) & ((1<< 5)-1);
			hour = (value_ >>	(60-26)) & ((1<< 5)-1);
			minute = (value_ >>	(60-33)) & ((1<< 7)-1);
			second = (value_ >>	(60-40)) & ((1<< 7)-1);
			millisecond = (value_ >>(60-50)) & ((1<<10)-1);
			microsecond = (value_ >>(60-60)) & ((1<<10)-1);
		}

		Timestamp value() const
		{
			Timestamp rt = 0;
			rt += (Timestamp)year		<< (60-12);
			rt += (Timestamp)month		<< (60-16);
			rt += (Timestamp)day		<< (60-21);
			rt += (Timestamp)hour		<< (60-26);
			rt += (Timestamp)minute		<< (60-33);
			rt += (Timestamp)second		<< (60-40);
			rt += (Timestamp)millisecond	<< (60-50);
			rt += (Timestamp)microsecond	<< (60-60);
			return rt;
		}
	};
	Impl m_impl;
};

}}//namespace
#endif


