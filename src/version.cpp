/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
//
// version.cpp
//

#include <string>
#include <sstream>
#include "version.hpp"

namespace _Wolframe {

Version::Version()
{
	major_ = minor_ = revision_ = build_ = 0;
	hasRevision_ = hasBuild_ = false;
}

Version::Version( unsigned short M, unsigned short m )
{
	major_ = M, minor_ = m;
	revision_ = build_ = 0;
	hasRevision_ = hasBuild_ = false;
}

Version::Version( unsigned short M, unsigned short m, unsigned short r )
{
	major_ = M, minor_ = m, revision_ = r;
	build_ = 0;
	hasRevision_ = true;
	hasBuild_ = false;
}

Version::Version( unsigned short M, unsigned short m, unsigned short r, unsigned b )
{
	major_ = M, minor_ = m, revision_ = r, build_ = b;
	hasRevision_ = hasBuild_ = true;
}


bool Version::operator== ( const Version &other ) const
{
	if ( major_ != other.major_ )		return false;
	if ( minor_ != other.minor_ )		return false;
	if ( revision_ != other.revision_ )	return false;
	if ( build_ != other.build_ )		return false;
	return true;
}

bool Version::operator> ( const Version &other ) const
{
	if ( major_ > other.major_ )		return true;
	if ( major_ < other.major_ )		return false;
	if ( minor_ > other.minor_ )		return true;
	if ( minor_ < other.minor_ )		return false;
	if ( revision_ > other.revision_ )	return true;
	if ( revision_ < other.revision_ )	return false;
	if ( build_ > other.build_ )		return true;

	return false;
}


std::string Version::toString() const
{
	std::ostringstream	o;

	o << major_ << "." << minor_;
	if ( hasRevision_ )	{
		o << "." << revision_;
		if ( hasBuild_ )	{
			o << "." << build_;
		}
	}
	return o.str();
}

std::string Version::toString( const char* format ) const
{
	std::ostringstream	o;
	bool			escaped = false;

	for ( const char *it = format; *it != '\0'; it++ )	{
		if ( escaped )	{
			switch( *it )	{
			case '%': o << *it; break;
			case 'M': o << major_; break;
			case 'm': o << minor_;	break;
			case 'r': if ( hasRevision_ ) o << revision_; break;
			case 'b': if ( hasBuild_ ) o << build_; break;
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

} // namespace _Wolframe
