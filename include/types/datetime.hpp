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
//\remark Only transport format. No date time arithmetics implemented here
class DateTime
{
public:
	enum SubType
	{
		YYYYMMDD,
		YYYYMMDDhhmmss,
		YYYYMMDDhhmmss_lll,
		YYYYMMDDhhmmss_lllccc
	};
	DateTime( const DateTime& o)
		:m_year(o.m_year),m_month(o.m_month),m_day(o.m_day),m_hour(o.m_hour),m_minute(o.m_minute),m_second(o.m_second),m_millisecond(o.m_millisecond),m_microsecond(o.m_microsecond),m_subtype(o.m_subtype)
	{}

	DateTime( unsigned short YY,
		  unsigned short MM,
		  unsigned short DD,
		  unsigned short hh,
		  unsigned short mm,
		  unsigned short ss,
		  unsigned short ll,
		  unsigned short cc)
		:m_year(YY),m_month(MM),m_day(DD),m_hour(hh),m_minute(mm),m_second(ss),m_millisecond(ll),m_microsecond(cc),m_subtype(YYYYMMDDhhmmss_lllccc)
	{
		check( YY, MM, DD, hh, mm, ss, ll, cc);
	}

	DateTime( unsigned short YY,
		  unsigned short MM,
		  unsigned short DD,
		  unsigned short hh,
		  unsigned short mm,
		  unsigned short ss,
		  unsigned short ll)
		:m_year(YY),m_month(MM),m_day(DD),m_hour(hh),m_minute(mm),m_second(ss),m_millisecond(ll),m_microsecond(0),m_subtype(YYYYMMDDhhmmss_lll)
	{
		check( YY, MM, DD, hh, mm, ss, ll, 0);
	}

	DateTime( unsigned short YY,
		  unsigned short MM,
		  unsigned short DD,
		  unsigned short hh,
		  unsigned short mm,
		  unsigned short ss)
		:m_year(YY),m_month(MM),m_day(DD),m_hour(hh),m_minute(mm),m_second(ss),m_millisecond(0),m_microsecond(0),m_subtype(YYYYMMDDhhmmss)
	{
		check( YY, MM, DD, hh, mm, ss, 0, 0);
	}

	DateTime( unsigned short YY,
		  unsigned short MM,
		  unsigned short DD)
		:m_year(YY),m_month(MM),m_day(DD),m_hour(0),m_minute(0),m_second(0),m_millisecond(0),m_microsecond(0),m_subtype(YYYYMMDD)
	{
		check( YY, MM, DD, 0, 0, 0, 0, 0);
	}

	unsigned int year() const		{return m_year;}		//< year 1000..2400
	unsigned int month() const		{return m_month;}		//< month 1..12
	unsigned int day() const		{return m_day;}			//< day 1..31
	unsigned int hour() const		{return m_hour;}		//< hour
	unsigned int minute() const		{return m_minute;}		//< minute
	unsigned int second() const		{return m_second;}		//< second
	unsigned int millisecond() const	{return m_millisecond;}		//< 1/1000 of second part
	unsigned int microsecond() const	{return m_microsecond;}		//< 1/1000 of millisecond part
	SubType subtype() const			{return (SubType)m_subtype;}	//< Granularity

	DateTime( const Timestamp& t)
	{
		init( t);
	}

	Timestamp value() const
	{
		Timestamp rt = 0;
		setBits( rt, m_year, 0, 13);
		setBits( rt, m_month, 13, 4);
		setBits( rt, m_day, 17, 5);
		setBits( rt, m_hour, 22, 5);
		setBits( rt, m_minute, 27, 6);
		setBits( rt, m_second, 33, 6);
		setBits( rt, m_millisecond, 39, 10);
		setBits( rt, m_millisecond, 49, 10);
		setBits( rt, m_subtype, 59, 2);
		return rt;
	}

	void init( Timestamp value_)
	{
		m_year = getBits( value_, 0, 13);
		m_month = getBits( value_, 13, 4);
		m_day = getBits( value_, 17, 5);
		m_hour = getBits( value_, 22, 5);
		m_minute = getBits( value_, 27, 6);
		m_second = getBits( value_, 33, 6);
		m_millisecond = getBits( value_, 39, 10);
		m_microsecond = getBits( value_, 49, 10);
		m_subtype = (SubType)getBits( value_, 59, 2);
	}

	std::string tostring() const
	{
		std::string rt;
		appendValue( rt, m_year, 4);
		appendValue( rt, m_month, 2);
		appendValue( rt, m_day, 2);
		if ((SubType)m_subtype == YYYYMMDD) return rt;
		appendValue( rt, m_hour, 2);
		appendValue( rt, m_minute, 2);
		appendValue( rt, m_second, 2);
		if ((SubType)m_subtype == YYYYMMDDhhmmss) return rt;
		appendValue( rt, m_millisecond, 3);
		if ((SubType)m_subtype == YYYYMMDDhhmmss_lll) return rt;
		appendValue( rt, m_microsecond, 3);
		return rt;
	}

private:
	static void setBits( Timestamp& dest, unsigned int val, unsigned int bitidx, unsigned int bitrange)
	{
		dest |= ((Timestamp) val << (63-bitidx-bitrange));
	}
	static unsigned int getBits( const Timestamp& from, unsigned int bitidx, unsigned int bitrange)
	{
		return (unsigned int)(from >> (63-bitidx-bitrange)) & ((1<<(bitrange+1))-1);
	}

	static void check_range( unsigned int val, unsigned int from, unsigned int to)
	{
		if (val < from || val > to) throw std::runtime_error("value out of range");
	}
	static void check_range( unsigned int val, unsigned int maximum)
	{
		if (val > maximum) throw std::runtime_error("value out of range");
	}
	static void check( unsigned short YY, unsigned short MM, unsigned short DD, unsigned short hh, unsigned short mm, unsigned short ss, unsigned short ll, unsigned short cc)
	{
		check_range( YY, 1000, 2400);
		check_range( MM, 1, 12);
		check_range( DD, 1, 31);
		check_range( hh, 64);
		check_range( mm, 64);
		check_range( ss, 64);
		check_range( ll, 1004);
		check_range( cc, 1004);
	}

	static void appendValue( std::string& dest, unsigned int value, unsigned int nofDigits)
	{
		enum {BufSize=8};
		unsigned char buf[BufSize];
		while (nofDigits != 0)
		{
			--nofDigits;
			unsigned char digit = (value % 10);
			value /= 10;
			buf[ nofDigits] = digit + '0';
			if (value == 0) while (nofDigits-- != 0) buf[ nofDigits] = '0';
		}
		dest.append( (char*)buf);
	}

private:
	unsigned int m_year:13;		//< [ 0..12]
	unsigned int m_month:4;		//< [13..16]
	unsigned int m_day:5;		//< [17..21]
	unsigned int m_hour:5;		//< [22..26]
	unsigned int m_minute:6;	//< [27..32]
	unsigned int m_second:6;	//< [33..39]
	unsigned int m_millisecond:10;	//< [39..49]
	unsigned int m_microsecond:10;	//< [49..59]
	unsigned int m_subtype:2;	//< [59..60]
};

}}//namespace
#endif


