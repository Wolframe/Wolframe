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
#include <limits>
#include <stdexcept>
#include "version.hpp"

namespace _Wolframe {

Version::Version( const char* version, const char* format)
	: m_major( 0 ), m_minor( 0 ),
	  m_revision( 0 ), m_hasRevision( false ),
	  m_build( 0 ), m_hasBuild( false )
{
	enum {NofElements=4};
	std::size_t vi=0,ai=0,fi=0;
	unsigned short ar[NofElements] = {0,0,0,0};
	unsigned int cnt[ NofElements] = {0,0,0,0};
	enum State {Parse,Shift};
	State state = Shift;

	while (version[vi] && ai < NofElements && ar[ai] < std::numeric_limits<unsigned short>::max())
	{
		switch (state)
		{
			case Shift:
				if (format[fi] == '%')
				{
					++fi;
					if (format[fi] != '%')
					{
						switch (format[fi])
						{
							case '|': ++fi; continue;
							case 'M': ai = 0; ++fi; break;
							case 'm': ai = 1; ++fi; break;
							case 'r': ai = 2; ++fi; break;
							case 'b': ai = 3; ++fi; break;
							default: throw std::runtime_error( "format string syntax error");
						}
						if (cnt[ai]) throw std::runtime_error( "duplicate entry in format string");
						cnt[ai] = 1;
						state = Parse;
						continue;
					}
				}
				if (format[fi] != version[vi]) throw std::runtime_error( "version string syntax error");
				++fi;
				++vi;
				break;

			case Parse:
				if (version[vi] >= '0' && version[vi] <= '9')
				{
					ar[ai] = ar[ai] * 10 + (version[vi] - '0');
					++cnt[ai];
					++vi;
				}
				else
				{
					if (cnt[ai] == 1) throw std::runtime_error( "empty element in version string");
					state = Shift;
					break;
				}
			break;
		}
	}
	if (version[vi])
	{
		if (!format[fi])
		{}
		else if (format[fi] == '%' && format[fi+1] == '|')
		{}
		else
		{
			throw std::runtime_error( "version string syntax error");
		}
	}
	if (cnt[0] && cnt[1])
	{
		if (cnt[2])
		{
			if (cnt[3])
			{
				m_build = ar[3];
				m_hasBuild = true;
			}
			m_revision = ar[2];
			m_hasRevision = true;
		}
		else if (cnt[3])
		{
			throw std::runtime_error( "version string not complete: Has build number but no revision");
		}
		m_major = ar[0];
		m_minor = ar[1];
	}
	else
	{
		throw std::runtime_error( "version string not complete");
	}
}

Version::Version( unsigned long version )
	: m_major((unsigned short)( version / 10000000lu )),
	  m_minor((unsigned short)(( version % 10000000lu ) / 100000 )),
	  m_revision( (unsigned short)(( version % 100000lu ) / 1000 )), m_hasRevision( true ),
	  m_build( (unsigned short)( version % 1000 )), m_hasBuild( true )
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
	return true;
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
