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
///\brief Date and datetime value type

#ifndef _Wolframe_TYPES_DATETIME_HPP_INCLUDED
#define _Wolframe_TYPES_DATETIME_HPP_INCLUDED
#include <string>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
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
	//\enum SubType
	//\brief Describing the format of the DateTime value
	enum SubType
	{
		YYYYMMDD,		//< date
		YYYYMMDDhhmmss,		//< date with time down to seconds
		YYYYMMDDhhmmss_lll,	//< date with time down to milliseconds (1/1'000 seconds)
		YYYYMMDDhhmmss_lllccc	//< date with time down to microseconds (1/1'000'000 seconds)
	};
	//\brief Copy constructor
	DateTime( const DateTime& o)
		:m_year(o.m_year),m_month(o.m_month),m_day(o.m_day),m_hour(o.m_hour),m_minute(o.m_minute),m_second(o.m_second),m_millisecond(o.m_millisecond),m_microsecond(o.m_microsecond),m_subtype(o.m_subtype)
	{}

	//\brief Constructor of datetime format YYYYMMDDhhmmss_lllccc
	//\param[in] YY 4 digits year (AD, e.g. 1968)
	//\param[in] MM month in year starting with 1
	//\param[in] DD day in month starting with 1
	//\param[in] hh hours part [0..23]
	//\param[in] mm minutes part [0..59]
	//\param[in] ss seconds part [0..63] (59 + eventual leap seconds)
	//\param[in] ll milliseconds part [0..999]
	//\param[in] cc microseconds part [0..999]
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

	//\brief Constructor of datetime format YYYYMMDDhhmmss_lll
	//\param[in] YY 4 digits year (AD, e.g. 1968)
	//\param[in] MM month in the year starting with 1
	//\param[in] DD day in the month starting with 1
	//\param[in] hh hours part [0..23]
	//\param[in] hh hours part [0..23]
	//\param[in] mm minutes part [0..59]
	//\param[in] ss seconds part [0..63] (59 + eventual leap seconds)
	//\param[in] ll milliseconds part [0..999]
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

	//\brief Constructor of datetime format YYYYMMDDhhmmss
	//\param[in] YY 4 digits year (AD, e.g. 1968)
	//\param[in] MM month in year starting with 1
	//\param[in] DD day in month starting with 1
	//\param[in] hh hours part [0..23]
	//\param[in] mm minutes part [0..59]
	//\param[in] ss seconds part [0..63] (59 + eventual leap seconds)
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

	//\brief Constructor of datetime format YYYYMMDD
	//\param[in] YY 4 digits year (AD, e.g. 1968)
	//\param[in] MM month in year starting with 1
	//\param[in] DD day in month starting with 1
	DateTime( unsigned short YY,
		  unsigned short MM,
		  unsigned short DD)
		:m_year(YY),m_month(MM),m_day(DD),m_hour(0),m_minute(0),m_second(0),m_millisecond(0),m_microsecond(0),m_subtype(YYYYMMDD)
	{
		check( YY, MM, DD, 0, 0, 0, 0, 0);
	}

	//\brief Get the year AD of the datetime
	//\return the year
	unsigned int year() const		{return m_year;}		//< year 1000..2400
	//\brief Get the month of the datetime in the year starting with 1
	//\return the month
	unsigned int month() const		{return m_month;}		//< month 1..12
	//\brief Get the day of the datetime in the month starting with 1
	//\return the day
	unsigned int day() const		{return m_day;}			//< day 1..31
	//\brief Get the hour of the datetime
	//\return the hour
	unsigned int hour() const		{return m_hour;}		//< hour
	//\brief Get the minutes part of the datetime
	//\return the minutes part
	unsigned int minute() const		{return m_minute;}		//< minute
	//\brief Get the seconds part of the datetime
	//\return the seconds part
	unsigned int second() const		{return m_second;}		//< second
	//\brief Get the milliseconds part of the datetime
	//\return the milliseconds part
	unsigned int millisecond() const	{return m_millisecond;}		//< 1/1000 of second part
	//\brief Get the microseconds part of the datetime
	//\return the microseconds part
	unsigned int microsecond() const	{return m_microsecond;}		//< 1/1000 of millisecond part
	//\brief Get the format (subtype) of the datetime
	//\return the subtype
	SubType subtype() const			{return (SubType)m_subtype;}	//< Granularity

	//\brief Constructor
	//\param[in] t timestamp value of the datetime
	DateTime( const Timestamp& t)
	{
		init( t);
	}
	//\brief Constructor
	//\param[in] s string representation of the datetime as YYYYMMDDhhmmsslllccc,YYYYMMDDhhmmsslll,YYYYMMDDhhmmss or YYYYMMDD depending on subtype (without any delimiter)
	DateTime( const std::string& s)
	{
		init( s);
	}

	//\brief Get the datetime timestamp value of the datetime
	//\return the timestamp
	Timestamp timestamp() const
	{
		Timestamp rt = 0;
		setBits( rt, m_year, 0, 13);
		setBits( rt, m_month, 13, 4);
		setBits( rt, m_day, 17, 5);
		setBits( rt, m_hour, 22, 5);
		setBits( rt, m_minute, 27, 6);
		setBits( rt, m_second, 33, 6);
		setBits( rt, m_millisecond, 39, 10);
		setBits( rt, m_microsecond, 49, 10);
		setBits( rt, m_subtype, 59, 2);
		return rt;
	}

	//\brief Initialize the datetime with its timestamp value
	//\param[in] timestamp_ the timestamp value
	void init( Timestamp timestamp_)
	{
		m_year = getBits( timestamp_, 0, 13);
		m_month = getBits( timestamp_, 13, 4);
		m_day = getBits( timestamp_, 17, 5);
		m_hour = getBits( timestamp_, 22, 5);
		m_minute = getBits( timestamp_, 27, 6);
		m_second = getBits( timestamp_, 33, 6);
		m_millisecond = getBits( timestamp_, 39, 10);
		m_microsecond = getBits( timestamp_, 49, 10);
		m_subtype = (SubType)getBits( timestamp_, 59, 2);
	}

	//\brief Initialize the datetime with its string value
	//\param[in] timestamp_ string representation of the datetime as YYYYMMDDhhmmsslllccc,YYYYMMDDhhmmsslll,YYYYMMDDhhmmss or YYYYMMDD depending on subtype (without any delimiter)
	void init( const std::string& timestamp_)
	{
		std::memset( this, 0, sizeof(*this));
		std::string::const_iterator ti = timestamp_.begin(), te = timestamp_.end();
		for (;ti != te; ++ti)
		{
			if (*ti < '0' || *ti > '9') throw std::runtime_error("illegal datetime format (only digits expected)");
		}
		switch (timestamp_.size())
		{
			case 20: m_subtype = YYYYMMDDhhmmss_lllccc; break;
			case 17: m_subtype = YYYYMMDDhhmmss_lll; break;
			case 14: m_subtype = YYYYMMDDhhmmss; break;
			case 8: m_subtype = YYYYMMDD; break;
			default: throw std::runtime_error("illegal datetime format (size)");
		}
		check_range( m_year = getSubstringAsInt( timestamp_, 0, 4), 1000, 2400);
		check_range( m_month = getSubstringAsInt( timestamp_, 4, 2), 1, 12);
		check_range( m_day = getSubstringAsInt( timestamp_, 6, 2), 1, 31);
		if ((SubType)m_subtype == YYYYMMDD) return;
		check_range( m_hour = getSubstringAsInt( timestamp_, 8, 2), 0, 23);
		check_range( m_minute = getSubstringAsInt( timestamp_, 10, 2), 0, 59);
		check_range( m_second = getSubstringAsInt( timestamp_, 12, 2), 0, 63);	//... 59 + eventual leap seconds
		if ((SubType)m_subtype == YYYYMMDDhhmmss) return;
		check_range( m_millisecond = getSubstringAsInt( timestamp_, 14, 3), 0, 999);
		if ((SubType)m_subtype == YYYYMMDDhhmmss_lll) return;
		check_range( m_microsecond = getSubstringAsInt( timestamp_, 17, 3), 0, 999);
	}

	//\brief Get the string value of the datetime
	//\return the string value
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

	//\brief Test argument datetime for equality
	//\return true if yes
	bool operator==( const DateTime& o) const	{return compare(o)==0;}
	//\brief Test argument datetime for inequality
	//\return true if yes
	bool operator!=( const DateTime& o) const	{return compare(o)!=0;}
	//\brief Test if this datetime is bigger or equal than argument
	//\return true if yes
	bool operator>=( const DateTime& o) const	{return compare(o)>=0;}
	//\brief Test if this datetime is bigger than argument
	//\return true if yes
	bool operator> ( const DateTime& o) const	{return compare(o)> 0;}
	//\brief Test if this datetime is smaller or equal than argument
	//\return true if yes
	bool operator<=( const DateTime& o) const	{return compare(o)<=0;}
	//\brief Test if this datetime is smaller than argument
	//\return true if yes
	bool operator< ( const DateTime& o) const	{return compare(o)< 0;}

	//\brief Compare with argument datetime
	//\return int {-1,0,+1} depending on comparison result
	int compare( const DateTime& o) const
	{
		SubType cmpsubtype = (SubType)((m_subtype > o.m_subtype)?o.m_subtype:m_subtype);
		int rt;
		if (0!=(rt=compareValue( m_year, o.m_year))) return rt;
		if (0!=(rt=compareValue( m_month, o.m_month))) return rt;
		if (0!=(rt=compareValue( m_day, o.m_day))) return rt;
		if (cmpsubtype == YYYYMMDD) return rt;
		if (0!=(rt=compareValue( m_hour, o.m_hour))) return rt;
		if (0!=(rt=compareValue( m_minute, o.m_minute))) return rt;
		if (0!=(rt=compareValue( m_second, o.m_second))) return rt;
		if (cmpsubtype == YYYYMMDDhhmmss) return rt;
		if (0!=(rt=compareValue( m_millisecond, o.m_millisecond))) return rt;
		if (cmpsubtype == YYYYMMDDhhmmss_lll) return rt;
		if (0!=(rt=compareValue( m_microsecond, o.m_microsecond))) return rt;
		return 0;
	}

private:
	//\brief Compare two integer values
	//\return int {-1,0,+1} depending on comparison result
	static int compareValue( unsigned int aa, unsigned int bb)
	{
		if (aa == bb) return 0;
		if (aa < bb) return -1;
		return +1;
	}
	//\brief Initialize the bits [bitidx..bitidx+bitrange-1] of a value
	//\param[out] dest value to write to
	//\param[in] val value to read from
	//\param[in] bitidx index of first bit in 'dest' starting with 0
	//\param[in] bitrange number of bits in 'dest' of the value to initialize
	static void setBits( Timestamp& dest, unsigned int val, unsigned int bitidx, unsigned int bitrange)
	{
		dest |= ((Timestamp) val << (63-bitidx-bitrange));
	}
	//\brief Get the bits [bitidx..bitidx+bitrange-1] of a value
	//\param[in] from value to read from
	//\param[in] bitidx index of first bit in 'from' starting with 0
	//\param[in] bitrange number of bits in 'from' of the value to read from
	static unsigned int getBits( const Timestamp& from, unsigned int bitidx, unsigned int bitrange)
	{
		return (unsigned int)(from >> (63-bitidx-bitrange)) & ((1<<(bitrange+1))-1);
	}
	//\brief Get the characters [idx..idx+bitrange-1] of a value as unsigned int
	//\param[in] from value to read from
	//\param[in] idx index of first character in 'from' starting with 0
	//\param[in] range number of characters in 'from' of the value to read from
	static unsigned int getSubstringAsInt( const std::string& from, unsigned int idx, unsigned int range)
	{
		return std::atoi( std::string( from, idx, range).c_str());
	}
	//\brief Check the value to be within the range of [from,to]
	//\param[in] val value to check
	//\param[in] from start (including) of the range to check
	//\param[in] to end (including) of the range to check
	//\remark throws if the check fails
	static void check_range( unsigned int val, unsigned int from, unsigned int to)
	{
		if (val < from || val > to)
		{
			throw std::runtime_error("value out of range");
		}
	}
	//\brief Check the value to be within the range of [0,to]
	//\param[in] val value to check
	//\param[in] maximum maximum valid value of the range to check
	//\remark throws if the check fails
	static void check_range( unsigned int val, unsigned int maximum)
	{
		if (val > maximum)
		{
			throw std::runtime_error("value out of range");
		}
	}
	//\brief Verify the date element without calendar validity checks
	//\param[in] YY 4 digits year (AD, e.g. 1968)
	//\param[in] MM month in the year starting with 1
	//\param[in] DD day in the month starting with 1
	//\param[in] hh hours part [0..23]
	//\param[in] mm minutes part
	//\param[in] ss seconds part
	//\param[in] ll milliseconds part
	//\param[in] cc microseconds part
	static void check( unsigned short YY, unsigned short MM, unsigned short DD, unsigned short hh, unsigned short mm, unsigned short ss, unsigned short ll, unsigned short cc)
	{
		check_range( YY, 1000, 2400);
		check_range( MM, 1, 12);
		check_range( DD, 1, 31);
		check_range( hh, 23);
		check_range( mm, 59);
		check_range( ss, 63);	//... 59 + eventual leap seconds
		check_range( ll, 999);
		check_range( cc, 999);
	}

	//\brief Append value formatted (filling zero's)
	//\param[out] dest result string to append to
	//\param[in] value value to append
	//\param[in] nofDigits number of digits to append (fill with leading zero's if value is smaller)
	static void appendValue( std::string& dest, unsigned int value, unsigned int nofDigits)
	{
		enum {BufSize=8};
		unsigned char buf[BufSize];
		buf[ nofDigits] = 0;

		while (nofDigits != 0)
		{
			--nofDigits;
			unsigned char digit = (value % 10);
			value /= 10;
			buf[ nofDigits] = digit + '0';
			if (value == 0)
			{
				while (nofDigits-- != 0) buf[ nofDigits] = '0';
				break;
			}
		}
		dest.append( (char*)buf);
	}

private:
	unsigned int m_year:13;		//< [ 0..12] 4 digits year AD
	unsigned int m_month:4;		//< [13..16] 2 digits month in year starting with 1
	unsigned int m_day:5;		//< [17..21] 2 digits day in month starting with 1
	unsigned int m_hour:5;		//< [22..26] hour of day [0..23]
	unsigned int m_minute:6;	//< [27..32] minutes part
	unsigned int m_second:6;	//< [33..39] seconds part
	unsigned int m_millisecond:10;	//< [39..49] milliseconds part
	unsigned int m_microsecond:10;	//< [49..59] micorseconds part
	unsigned int m_subtype:2;	//< [59..60] subtype (format) of the datetime
};

}}//namespace
#endif


