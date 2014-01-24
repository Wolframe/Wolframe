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
///\file dateArithmetic.cpp
///\brief Date arithmetic functions
#include "dateArithmetic.hpp"
#include <stdexcept>
#include <cstring>

static const char* ISO_extended_format = "%Y-%m-%d";		//< default format for dates

using namespace _Wolframe;
using namespace _Wolframe::types;

Date::Date( unsigned short y, unsigned short m, unsigned short d)
	:m_date( y, m, d){}

Date::Date( const std::string& dt, const char* format)
{
	std::istringstream dti;
	dti.str( dt);
	boost::gregorian::date_input_facet *dtf = new boost::gregorian::date_input_facet( format?format:ISO_extended_format);
	dti.imbue( std::locale( std::locale::classic(), dtf));
	dti >> m_date;
}

std::string Date::tostring( const char* format) const
{
	std::ostringstream dto;
	boost::gregorian::date_facet* dtf = new boost::gregorian::date_facet( format?format:ISO_extended_format);
	dto.imbue( std::locale( std::locale::classic(), dtf));
	dto << m_date;
	return dto.str();
}

long Date::operator - (const Date& o) const
{
	return (m_date - o.m_date).days();
}

Date Date::operator + (long days) const
{
	boost::gregorian::date_duration d( days);
	return Date( m_date + d);
}

Date Date::operator - (long days) const
{
	boost::gregorian::date_duration d( days);
	return Date( m_date - d);
}

Date& Date::operator += (long days)
{
	boost::gregorian::date_duration d( days);
	m_date += d;
	return *this;
}

Date& Date::operator -= (long days)
{
	boost::gregorian::date_duration d( days);
	m_date -= d;
	return *this;
}


