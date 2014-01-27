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
///\file dateArithmetic.hpp
///\brief Interface to date arithmetic functions
#ifndef _TYPES_DATATYPE_DATE_ARITHMETIC_HPP_INCLUDED
#define _TYPES_DATATYPE_DATE_ARITHMETIC_HPP_INCLUDED
#include <string>
#include "boost/date_time/gregorian/gregorian.hpp"

namespace _Wolframe {
namespace types {

///\class Date
///\brief Type for date arithmetic
class Date
{
public:
	///\brief Constructor
	///\param[in] y number of years. Smallest year allowed in 1600
	///\param[in] m number of month counted from 1 (January)
	///\param[in] d number of days counted from 1 (first day of month)
	Date( unsigned short y, unsigned short m, unsigned short d);

	///\brief Constructor
	///\param[in] dt date as string
	///\param[in] format format definition of 'dt' in a format similar to printf
	///\remark For the format defintion see http://www.boost.org/doc/libs/1_43_0/doc/html/date_time/date_time_io.html
	explicit Date( const std::string& dt, const char* format=0);

	///\brief Copy constructor
	Date( const Date& o)
		:m_date(o.m_date){}

	///\brief Default constructor
	Date()
		:m_date(boost::gregorian::day_clock::local_day()){}

	///\brief Assignment operator
	Date& operator=( const Date& o)			{m_date = o.m_date; return *this;}

	//\brief Get the year part of the date (YYYY)
	int year() const				{return m_date.year();}
	//\brief Get the month part of the date ( >= 1)
	int month() const				{return m_date.month();}
	//\brief Get the day part of the date ( >= 1)
	int day() const					{return m_date.day();}

	///\brief Get the difference in days
	long operator - (const Date& o) const;
	///\brief Add number of days
	Date operator + (long days) const;
	///\brief Subtract number of days
	Date operator - (long days) const;
	///\brief Add number of days
	Date& operator += (long days);
	///\brief Subtract number of days
	Date& operator -= (long days);

	bool operator == (const Date& o) const		{return m_date == o.m_date;}
	bool operator != (const Date& o) const		{return m_date != o.m_date;}
	bool operator >= (const Date& o) const		{return m_date >= o.m_date;}
	bool operator <= (const Date& o) const		{return m_date <= o.m_date;}
	bool operator < (const Date& o) const		{return m_date < o.m_date;}
	bool operator > (const Date& o) const		{return m_date > o.m_date;}

	///\brief Get the date as a formated string
	///\param[in] format format definition of 'dt' in a format similar to printf
	///\return the date as string
	///\remark For the format defintion see http://www.boost.org/doc/libs/1_43_0/doc/html/date_time/date_time_io.html
	std::string tostring( const char* format) const;
	std::string tostring() const			{return tostring(0);}

private:
	///\brief Constructor
	Date( const boost::gregorian::date& o)
		:m_date(o){}

	///\brief Internal representation of the date
	boost::gregorian::date m_date;
};


}}
#endif

