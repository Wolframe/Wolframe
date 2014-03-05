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
///\file types_datetime.cpp
///\brief Implementation of date and datetime value type
#include "types/datetime.hpp"
#include "types/variant.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>

using namespace _Wolframe;
using namespace _Wolframe::types;

DateTime::DateTime( const Variant& v)
{
	if (v.type() == Variant::Timestamp)
	{
		init( (Timestamp)v.data().value.Timestamp);
	}
	else
	{
		init( v.tostring());
	}
}

Timestamp DateTime::timestamp() const
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
	setBits( rt, m_subtype, 59, 3);
	return rt;
}

void DateTime::init( Timestamp timestamp_)
{
	m_year = (unsigned short)getBits( timestamp_, 0, 13);
	m_month = (unsigned char)getBits( timestamp_, 13, 4);
	m_day = (unsigned char)getBits( timestamp_, 17, 5);
	m_hour = (unsigned char)getBits( timestamp_, 22, 5);
	m_minute = (unsigned char)getBits( timestamp_, 27, 6);
	m_second = (unsigned char)getBits( timestamp_, 33, 6);
	m_millisecond = (unsigned short)getBits( timestamp_, 39, 10);
	m_microsecond = (unsigned short)getBits( timestamp_, 49, 10);
	m_subtype = (SubType)getBits( timestamp_, 59, 3);
}


void DateTime::init( const char* str, std::size_t strsize)
{
	std::memset( this, 0, sizeof(*this));
	std::size_t ti=0,te=strsize;
	std::string buf;

	for (;ti != te; ++ti)
	{
		if (str[ti] < '0' || str[ti] > '9')
		{
			if (buf.size() == 8 && str[ti] == 'T') continue;
			if ((buf.size() == 4 || buf.size() == 6) && str[ti] == '-') continue;
			if ((buf.size() == 10 || buf.size() == 12) && str[ti] == ':') continue;
			if ((buf.size() == 14) && str[ti] == ',') continue;
			if (buf.size() == 8 && str[ti] == ' ') continue;

			throw std::runtime_error("illegal datetime format");
		}
		buf.push_back( str[ti]);
	}
	switch (buf.size())
	{
		case 20: m_subtype = YYYYMMDDhhmmss_lllccc; break;
		case 17: m_subtype = YYYYMMDDhhmmss_lll; break;
		case 14: m_subtype = YYYYMMDDhhmmss; break;
		case 8: m_subtype = YYYYMMDD; break;
		default: throw std::runtime_error("illegal datetime format (size)");
	}
	check_range( m_year = (unsigned short)getSubstringAsInt( buf.c_str(), 0, 4), 1000, 2400);
	check_range( m_month = (unsigned char)getSubstringAsInt( buf.c_str(), 4, 2), 1, 12);
	check_range( m_day = (unsigned char)getSubstringAsInt( buf.c_str(), 6, 2), 1, 31);
	if ((SubType)m_subtype == YYYYMMDD) return;
	check_range( m_hour = (unsigned char)getSubstringAsInt( buf.c_str(), 8, 2), 0, 23);
	check_range( m_minute = (unsigned char)getSubstringAsInt( buf.c_str(), 10, 2), 0, 59);
	check_range( m_second = (unsigned char)getSubstringAsInt( buf.c_str(), 12, 2), 0, 63);	//... 59 + eventual leap seconds
	if ((SubType)m_subtype == YYYYMMDDhhmmss) return;
	check_range( m_millisecond = (unsigned short)getSubstringAsInt( buf.c_str(), 14, 3), 0, 999);
	if ((SubType)m_subtype == YYYYMMDDhhmmss_lll) return;
	check_range( m_microsecond = (unsigned short)getSubstringAsInt( buf.c_str(), 17, 3), 0, 999);
}

void DateTime::init( const std::string& str_)
{
	init( str_.c_str(), str_.size());
}

std::string DateTime::tostring( StringFormat::Id sf) const
{
	
	std::string rt;
	switch (sf)
	{
		case StringFormat::ISOdateTime:
		case StringFormat::YYYYMMDDhhmmssxxxxxx:
			appendValue( rt, m_year, 4);
			appendValue( rt, m_month, 2);
			appendValue( rt, m_day, 2);
			if ((SubType)m_subtype == YYYYMMDD) return rt;
			if (sf == StringFormat::ISOdateTime) rt.push_back( 'T');
			appendValue( rt, m_hour, 2);
			appendValue( rt, m_minute, 2);
			appendValue( rt, m_second, 2);
			if ((SubType)m_subtype == YYYYMMDDhhmmss) return rt;
			if (sf == StringFormat::ISOdateTime) rt.push_back( ',');
			appendValue( rt, m_millisecond, 3);
			if ((SubType)m_subtype == YYYYMMDDhhmmss_lll) return rt;
			appendValue( rt, m_microsecond, 3);
			break;
		case StringFormat::ExtendedISOdateTime:
			appendValue( rt, m_year, 4);
			rt.push_back( '-');
			appendValue( rt, m_month, 2);
			rt.push_back( '-');
			appendValue( rt, m_day, 2);
			if ((SubType)m_subtype == YYYYMMDD) return rt;
			rt.push_back( ' ');
			appendValue( rt, m_hour, 2);
			rt.push_back( ':');
			appendValue( rt, m_minute, 2);
			rt.push_back( ':');
			appendValue( rt, m_second, 2);
			if ((SubType)m_subtype == YYYYMMDDhhmmss) return rt;
			rt.push_back( ',');
			appendValue( rt, m_millisecond, 3);
			if ((SubType)m_subtype == YYYYMMDDhhmmss_lll) return rt;
			appendValue( rt, m_microsecond, 3);
			break;
	}
	return rt;
}

double DateTime::toMSDNtimestamp() const
{
	double rt = (long)(boost::gregorian::date( m_year, m_month, m_day) - boost::gregorian::date( 1899, 12, 30)).days();
	rt += (double) m_hour / 24.0;
	rt += (double) m_minute / (24.0 * 60);
	rt += (double) m_second / (24.0 * 60 * 60);
	rt += (double) m_millisecond / (24.0 * 60 * 60 * 1000);
	rt += (double) m_microsecond / (24.0 * 60 * 60 * 1000 * 1000);
	return rt;
}

DateTime DateTime::fromMSDNtimestamp( double tm)
{
	double fl = std::floor( tm);
	long days = (long)fl;
	double partofday = tm - fl;
	boost::gregorian::date dt = boost::gregorian::date( 1899, 12, 30) + boost::gregorian::date_duration(days);
	unsigned short hrs = (unsigned short)std::floor( partofday * 24);
	unsigned short mns = (unsigned short)std::floor( partofday * 24 * 60);
	unsigned short scs = (unsigned short)std::floor( partofday * 24 * 60 * 60);

	return DateTime( dt.year(), dt.month(), dt.day(), hrs, mns, scs);
}

int DateTime::compare( const DateTime& o) const
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

void DateTime::check( unsigned short YY, unsigned short MM, unsigned short DD, unsigned short hh, unsigned short mm, unsigned short ss, unsigned short ll, unsigned short cc)
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

void DateTime::appendValue( std::string& dest, unsigned int value, unsigned int nofDigits)
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

