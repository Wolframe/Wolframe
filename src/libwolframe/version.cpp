/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
/// \file  version.cpp
/// \brief Version class implementation

#include <string>
#include <sstream>
#include "version.hpp"

namespace _Wolframe {

/// This will contruct a Version object having major and minor versions set to 0,
/// no revision number and no build number.
Version::Version()
	: m_major( 0 ), m_minor( 0 ),
	  m_revision( 0 ), m_hasRevision( false ),
	  m_build( 0 ), m_hasBuild( false )
{}

Version::Version( unsigned long version )
	: m_major( version / 10000000lu ),
	  m_minor(( version % 10000000lu ) / 100000 ),
	  m_revision( ( version % 100000lu ) / 1000 ), m_hasRevision( true ),
	  m_build( version % 1000 ), m_hasBuild( true )
{}

Version::Version( unsigned short M, unsigned short m )
	: m_major( M ), m_minor( m ),
	  m_revision( 0 ), m_hasRevision( false ),
	  m_build( 0 ), m_hasBuild( false )
{}

Version::Version( unsigned short M, unsigned short m, unsigned short r )
	: m_major( M ), m_minor( m ),
	  m_revision( r ), m_hasRevision( true ),
	  m_build( 0 ), m_hasBuild( false )
{}

Version::Version( unsigned short M, unsigned short m, unsigned short r, unsigned b )
	: m_major( M ), m_minor( m ),
	  m_revision( r ), m_hasRevision( true ),
	  m_build( b ), m_hasBuild( true )
{}


bool Version::operator == ( const Version &other ) const
{
	if ( m_major != other.m_major )		return false;
	if ( m_minor != other.m_minor )		return false;
	if ( m_revision != other.m_revision )	return false;
	if ( m_build != other.m_build )		return false;
	return true;
}

bool Version::operator > ( const Version &other ) const
{
	if ( m_major > other.m_major )		return true;
	if ( m_major < other.m_major )		return false;
	if ( m_minor > other.m_minor )		return true;
	if ( m_minor < other.m_minor )		return false;
	if ( m_revision > other.m_revision )	return true;
	if ( m_revision < other.m_revision )	return false;
	if ( m_build > other.m_build )		return true;

	return false;
}

bool Version::isCompatible( const Version &other ) const
{
	if ( m_major != other.m_major )		return false;
	if ( m_minor > other.m_minor )		return false;
}

std::string Version::toString() const
{
	std::ostringstream	o;

	o << m_major << "." << m_minor;
	if ( m_hasRevision )	{
		o << "." << m_revision;
		if ( m_hasBuild )	{
			o << "." << m_build;
		}
	}
	return o.str();
}

/// The format string elements: (% is the escape character)
/// \li %% - %
/// \li %M - major version
/// \li %m - minor version
/// \li %r - revision
/// \li %b - build
/// \li all other characters are send directly to the output
std::string Version::toString( const char* format ) const
{
	std::ostringstream	o;
	bool			escaped = false;

	for ( const char *it = format; *it != '\0'; it++ )	{
		if ( escaped )	{
			switch( *it )	{
				case '%': o << *it; break;
				case 'M': o << m_major; break;
				case 'm': o << m_minor;	break;
				case 'r': if ( m_hasRevision ) o << m_revision; break;
				case 'b': if ( m_hasBuild ) o << m_build; break;
				default: o << *it;
			}
			escaped = false;
		}
		else	{
			if ( *it == '%' )
				escaped = true;
			else
				o << *it;
		}
	}
	return o.str();
}


unsigned long Version::toNumber() const
{
	unsigned	bld = 0;
	unsigned short	rev = 0;
	unsigned short	mnr = 0;
	unsigned short	mjr = 0;

	if ( m_hasBuild )	{
		bld = m_build;
		while( bld > 1000 )
			bld /= 10;
	}

	if ( m_hasRevision )	{
		rev = m_revision;
		while( rev > 100 )
			rev /= 10;
	}

	mnr = m_minor;
	while( mnr > 100 )
		mnr /= 10;

	mjr = m_major;
	while( mjr > 100 )
		mjr /= 10;

	return bld + rev * 1000 + mnr * 100000 + mjr * 10000000;
}

} // namespace _Wolframe
