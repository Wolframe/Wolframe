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
///\file types/dateArithmetic.hpp
///\brief Interface to date arithmetic functions
///\remark based on http://alcor.concordia.ca/~gpkatch/gdate-algorithm.html: Date arithmetics algorithm's described and implemented by Gary Katch (Implementation http://alcor.concordia.ca/~gpkatch/gdate-c.html)
#ifndef _TYPES_DATE_ARITHMETIC_HPP_INCLUDED
#define _TYPES_DATE_ARITHMETIC_HPP_INCLUDED
#include <string>

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
	Date( const std::string& dt, const char* format="%Y/%m/%d");

	///\brief Copy constructor
	Date( const Date& o)				:m_daynum(o.m_daynum){}

	///\brief Get the difference in days
	int operator - (const Date& o)			{return (int)(m_daynum - o.m_daynum);}
	///\brief Add number of days
	Date operator + (int days)			{Date rt( m_daynum + days); rt.check(); return rt;}
	///\brief Subtract number of days
	Date operator - (int days)			{Date rt( m_daynum - days); rt.check(); return rt;}
	///\brief Add number of days
	Date& operator += (int days)			{m_daynum += days; check(); return *this;}
	///\brief Subtract number of days
	Date& operator -= (int days)			{m_daynum -= days; check(); return *this;}

	///\brief Get the date as a formated string
	///\param[in] format format definition of 'dt' in a format similar to printf
	///\return the date as string
	///\remark For the format defintion see http://www.boost.org/doc/libs/1_43_0/doc/html/date_time/date_time_io.html
	std::string tostring( const char* format="%Y/%m/%d") const;

private:
	///\brief Check the date and throw an exception if the date is not in the boundaries allowed by this library (bigger than 1599/12/31)
	void check();
	///\brief Constructor
	Date( long daynum)
		:m_daynum(daynum){}

	///\brief Representation as number of days
	long m_daynum;
};


}}
#endif

