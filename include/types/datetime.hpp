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

///\brief Forward declaration
class Variant;

///\brief Timestamp equivalent to a date time value for variant type
typedef boost::uint64_t Timestamp;

///\class DateTime
///\brief Data type for normalized date time (absolute time without time zone info)
///\remark Only transport format. No date time arithmetics implemented here
class DateTime
{
public:
	///\enum SubType
	///\brief Describing the format of the DateTime value
	enum SubType
	{
		YYYYMMDD,		//< date
		YYYYMMDDhhmmss,		//< date with time down to seconds
		YYYYMMDDhhmmss_lll,	//< date with time down to milliseconds (1/1'000 seconds)
		YYYYMMDDhhmmss_lllccc	//< date with time down to microseconds (1/1'000'000 seconds)
	};
	///\brief Copy constructor
	DateTime( const DateTime& o)
		:m_year(o.m_year),m_month(o.m_month),m_day(o.m_day),m_hour(o.m_hour),m_minute(o.m_minute),m_second(o.m_second),m_millisecond(o.m_millisecond),m_microsecond(o.m_microsecond),m_subtype(o.m_subtype)
	{}

	///\brief Constructor of datetime format YYYYMMDDhhmmss_lllccc
	///\param[in] YY 4 digits year (AD, e.g. 1968)
	///\param[in] MM month in year starting with 1
	///\param[in] DD day in month starting with 1
	///\param[in] hh hours part [0..23]
	///\param[in] mm minutes part [0..59]
	///\param[in] ss seconds part [0..63] (59 + eventual leap seconds)
	///\param[in] ll milliseconds part [0..999]
	///\param[in] cc microseconds part [0..999]
	DateTime( unsigned short YY,
		  unsigned short MM,
		  unsigned short DD,
		  unsigned short hh,
		  unsigned short mm,
		  unsigned short ss,
		  unsigned short ll,
		  unsigned short cc)
		:m_year(YY),m_month((unsigned char)MM),m_day((unsigned char)DD),m_hour((unsigned char)hh),m_minute((unsigned char)mm),m_second((unsigned char)ss),m_millisecond(ll),m_microsecond(cc),m_subtype(YYYYMMDDhhmmss_lllccc)
	{
		check( YY, MM, DD, hh, mm, ss, ll, cc);
	}

	///\brief Constructor of datetime format YYYYMMDDhhmmss_lll
	///\param[in] YY 4 digits year (AD, e.g. 1968)
	///\param[in] MM month in the year starting with 1
	///\param[in] DD day in the month starting with 1
	///\param[in] hh hours part [0..23]
	///\param[in] hh hours part [0..23]
	///\param[in] mm minutes part [0..59]
	///\param[in] ss seconds part [0..63] (59 + eventual leap seconds)
	///\param[in] ll milliseconds part [0..999]
	DateTime( unsigned short YY,
		  unsigned short MM,
		  unsigned short DD,
		  unsigned short hh,
		  unsigned short mm,
		  unsigned short ss,
		  unsigned short ll)
		:m_year(YY),m_month((unsigned char)MM),m_day((unsigned char)DD),m_hour((unsigned char)hh),m_minute((unsigned char)mm),m_second((unsigned char)ss),m_millisecond(ll),m_microsecond(0),m_subtype(YYYYMMDDhhmmss_lll)
	{
		check( YY, MM, DD, hh, mm, ss, ll, 0);
	}

	///\brief Constructor of datetime format YYYYMMDDhhmmss
	///\param[in] YY 4 digits year (AD, e.g. 1968)
	///\param[in] MM month in year starting with 1
	///\param[in] DD day in month starting with 1
	///\param[in] hh hours part [0..23]
	///\param[in] mm minutes part [0..59]
	///\param[in] ss seconds part [0..63] (59 + eventual leap seconds)
	DateTime( unsigned short YY,
		  unsigned short MM,
		  unsigned short DD,
		  unsigned short hh,
		  unsigned short mm,
		  unsigned short ss)
		:m_year(YY),m_month((unsigned char)MM),m_day((unsigned char)DD),m_hour((unsigned char)hh),m_minute((unsigned char)mm),m_second((unsigned char)ss),m_millisecond(0),m_microsecond(0),m_subtype(YYYYMMDDhhmmss)
	{
		check( YY, MM, DD, hh, mm, ss, 0, 0);
	}

	///\brief Constructor of datetime format YYYYMMDD
	///\param[in] YY 4 digits year (AD, e.g. 1968)
	///\param[in] MM month in year starting with 1
	///\param[in] DD day in month starting with 1
	DateTime( unsigned short YY,
		  unsigned short MM,
		  unsigned short DD)
		:m_year(YY),m_month((unsigned char)MM),m_day((unsigned char)DD),m_hour(0),m_minute(0),m_second(0),m_millisecond(0),m_microsecond(0),m_subtype(YYYYMMDD)
	{
		check( YY, MM, DD, 0, 0, 0, 0, 0);
	}

	///\brief Get the year AD of the datetime
	///\return the year (year 1000..2400)
	unsigned int year() const		{return m_year;}
	///\brief Get the month of the datetime in the year starting with 1
	///\return the month (1..12)
	unsigned int month() const		{return m_month;}
	///\brief Get the day of the datetime in the month starting with 1
	///\return the day (1..31)
	unsigned int day() const		{return m_day;}
	///\brief Get the hour of the datetime
	///\return the hour
	unsigned int hour() const		{return m_hour;}
	///\brief Get the minutes part of the datetime
	///\return the minutes part
	unsigned int minute() const		{return m_minute;}
	///\brief Get the seconds part of the datetime
	///\return the seconds part
	unsigned int second() const		{return m_second;}
	///\brief Get the milliseconds part of the datetime
	///\return the milliseconds part (1/1000 of second part)
	unsigned int millisecond() const	{return m_millisecond;}
	///\brief Get the microseconds part of the datetime
	///\return the microseconds part (1/1000 of millisecond part)
	unsigned int microsecond() const	{return m_microsecond;}
	///\brief Get the format (subtype) of the datetime
	///\return the subtype (1/1000000 of second part)
	unsigned int usecond() const		{return m_second * 1000 + m_microsecond;}
	///\brief Get the format (subtype) of the datetime
	///\return the subtype (date/time granularity)
	SubType subtype() const			{return m_subtype;}

	///\brief Constructor
	///\param[in] t timestamp value of the datetime
	DateTime( const Timestamp& t)
	{
		init( t);
	}
	///\brief Constructor
	///\param[in] s string representation of the datetime as YYYYMMDDhhmmsslllccc,YYYYMMDDhhmmsslll,YYYYMMDDhhmmss or YYYYMMDD depending on subtype (without any delimiter)
	DateTime( const std::string& s)
	{
		init( s);
	}

	///\brief Constructor
	///\param[in] str string representation of the datetime as YYYYMMDDhhmmsslllccc,YYYYMMDDhhmmsslll,YYYYMMDDhhmmss or YYYYMMDD depending on subtype (without any delimiter)
	///\param[in] strsize size of 'str' in bytes
	DateTime( const char* str, std::size_t strsize)
	{
		init( str, strsize);
	}

	///\brief Constructor
	///\param[in] v variant representation of the datetime
	DateTime( const Variant& v);

	///\brief Get the datetime timestamp value of the datetime
	///\return the timestamp
	Timestamp timestamp() const;

	///\brief Initialize the datetime with its timestamp value
	///\param[in] timestamp_ the timestamp value
	void init( Timestamp timestamp_);

	///\brief Initialize the datetime with its string value
	///\remark See allowed formats in 'DateTime::init(const char*,std::size_t)'
	///\param[in] str string representation of the datetime as YYYYMMDDhhmmsslllccc,YYYYMMDDhhmmsslll,YYYYMMDDhhmmss or YYYYMMDD depending on subtype (without any delimiter)
	void init( const std::string& str);

	///\brief Initialize the datetime with its string value
	//	The date time value gets a subtype depending on what is specified in the string (e.g. if the granularity in the parsed string is seconds then the subtype gets YYYYMMDDhhmmss)
	///\remark Allowed formats:
	//	a) ISO date time 20011126T234312,123134
	//	b) 2001-11-26 23:43:12,123134
	//	c) 20011126234312123134
	///\remark Timezone not respected yet
	///\param[in] str string representation (see allowed formats)
	///\param[in] strsize size of 'str' in bytes
	void init( const char* str, std::size_t strsize);

	struct StringFormat
	{
		enum Id
		{
			YYYYMMDDhhmmssxxxxxx,
			ISOdateTime,
			ExtendedISOdateTime
		};
	};

	///\brief Get the MSDN DATE type from the date
	///\return days with fractional part of day for time from the 30th December 1899
	double toMSDNtimestamp() const;

	///\brief Get the date and time from an MSDN DATE type as YYYYMMDDhhmmss
	///\return The date and time value
	static DateTime fromMSDNtimestamp( double tm);

	///\brief Get the string value of the datetime
	///\param[in] sf format to print the date with
	///\return the string value
	std::string tostring( StringFormat::Id sf=StringFormat::YYYYMMDDhhmmssxxxxxx) const;

	///\brief Test argument datetime for equality
	///\return true if yes
	bool operator==( const DateTime& o) const	{return compare(o)==0;}
	///\brief Test argument datetime for inequality
	///\return true if yes
	bool operator!=( const DateTime& o) const	{return compare(o)!=0;}
	///\brief Test if this datetime is bigger or equal than argument
	///\return true if yes
	bool operator>=( const DateTime& o) const	{return compare(o)>=0;}
	///\brief Test if this datetime is bigger than argument
	///\return true if yes
	bool operator> ( const DateTime& o) const	{return compare(o)> 0;}
	///\brief Test if this datetime is smaller or equal than argument
	///\return true if yes
	bool operator<=( const DateTime& o) const	{return compare(o)<=0;}
	///\brief Test if this datetime is smaller than argument
	///\return true if yes
	bool operator< ( const DateTime& o) const	{return compare(o)< 0;}

	///\brief Compare with argument datetime
	///\return int {-1,0,+1} depending on comparison result
	int compare( const DateTime& o) const;

private:
	///\brief Compare two integer values
	///\return int {-1,0,+1} depending on comparison result
	static int compareValue( unsigned int aa, unsigned int bb)
	{
		if (aa == bb) return 0;
		if (aa < bb) return -1;
		return +1;
	}
	///\brief Initialize the bits [bitidx..bitidx+bitrange-1] of a value
	///\param[out] dest value to write to
	///\param[in] val value to read from
	///\param[in] bitidx index of first bit in 'dest' starting with 0
	///\param[in] bitrange number of bits in 'dest' of the value to initialize
	static void setBits( Timestamp& dest, unsigned int val, unsigned int bitidx, unsigned int bitrange)
	{
		dest |= ((Timestamp) val << (63-bitidx-bitrange));
	}
	///\brief Get the bits [bitidx..bitidx+bitrange-1] of a value
	///\param[in] from value to read from
	///\param[in] bitidx index of first bit in 'from' starting with 0
	///\param[in] bitrange number of bits in 'from' of the value to read from
	static unsigned int getBits( const Timestamp& from, unsigned int bitidx, unsigned int bitrange)
	{
		return (unsigned int)(from >> (63-bitidx-bitrange)) & ((1<<bitrange)-1);
	}
	///\brief Get the characters [idx..idx+bitrange-1] of a value as unsigned int
	///\param[in] from value to read from
	///\param[in] idx index of first character in 'from' starting with 0
	///\param[in] range number of characters in 'from' of the value to read from
	static unsigned int getSubstringAsInt( const char* from, unsigned int idx, unsigned int range)
	{
		return std::atoi( std::string( from + idx, range).c_str());
	}
	///\brief Check the value to be within the range of [from,to]
	///\param[in] val value to check
	///\param[in] from start (including) of the range to check
	///\param[in] to end (including) of the range to check
	///\remark throws if the check fails
	static void check_range( unsigned int val, unsigned int from, unsigned int to)
	{
		if (val < from || val > to)
		{
			throw std::runtime_error("value out of range");
		}
	}
	///\brief Check the value to be within the range of [0,to]
	///\param[in] val value to check
	///\param[in] maximum maximum valid value of the range to check
	///\remark throws if the check fails
	static void check_range( unsigned int val, unsigned int maximum)
	{
		if (val > maximum)
		{
			throw std::runtime_error("value out of range");
		}
	}
	///\brief Verify the date element without calendar validity checks
	///\param[in] YY 4 digits year (AD, e.g. 1968)
	///\param[in] MM month in the year starting with 1
	///\param[in] DD day in the month starting with 1
	///\param[in] hh hours part [0..23]
	///\param[in] mm minutes part
	///\param[in] ss seconds part
	///\param[in] ll milliseconds part
	///\param[in] cc microseconds part
	static void check( unsigned short YY, unsigned short MM, unsigned short DD, unsigned short hh, unsigned short mm, unsigned short ss, unsigned short ll, unsigned short cc);

	///\brief Append value formatted (filling zero's)
	///\param[out] dest result string to append to
	///\param[in] value value to append
	///\param[in] nofDigits number of digits to append (fill with leading zero's if value is smaller)
	static void appendValue( std::string& dest, unsigned int value, unsigned int nofDigits);

private:
	unsigned short m_year;		//< [ 0..12] 4 digits year AD
	unsigned char m_month;		//< [13..16] 2 digits month in year starting with 1
	unsigned char m_day;		//< [17..21] 2 digits day in month starting with 1
	unsigned char m_hour;		//< [22..26] hour of day [0..23]
	unsigned char m_minute;		//< [27..32] minutes part
	unsigned char m_second;		//< [33..39] seconds part
	unsigned short m_millisecond;	//< [39..49] milliseconds part
	unsigned short m_microsecond;	//< [49..59] micorseconds part
	SubType m_subtype;		//< [59..60] subtype (format) of the datetime
};

}}//namespace
#endif


