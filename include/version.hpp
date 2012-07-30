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
/**
 * Header file for the Version class
 */

#ifndef _VERSION_HPP_INCLUDED
#define _VERSION_HPP_INCLUDED

#include <string>

namespace _Wolframe {

class Version {
private:
	unsigned short	m_major;
	unsigned short	m_minor;
	unsigned short	m_revision;
	bool		m_hasRevision;
	unsigned	m_build;
	bool		m_hasBuild;
public:
	/// Empty Version constructor.
	///
	/// This will contruct a Version object having major and minor versions set to 0,
	/// no revision number and no build number.
	Version();
	Version( unsigned short M, unsigned short m );
	Version( unsigned short M, unsigned short m, unsigned short r );
	Version( unsigned short M, unsigned short m, unsigned short r, unsigned b );

	// Aba: clashes on FreeBSD with sys/types.h major and minor
	unsigned short Major() const		{ return m_major; }
	unsigned short Minor() const		{ return m_minor; }
	unsigned short Revision() const		{ return m_revision; }
	unsigned Build() const			{ return m_build; }

	bool operator == ( const Version &other ) const;
	bool operator != ( const Version &other ) const	{ return !( *this == other ); }

	bool operator >  ( const Version &other ) const;
	bool operator >= ( const Version &other ) const	{ return !( *this < other ); }
	bool operator <  ( const Version &other ) const	{ return  ( other > *this ); }
	bool operator <= ( const Version &other ) const	{ return !( *this > other ); }

	std::string toString() const;

	/// format string: (% is the escape character)
	/// %% - %
	/// %M - major version
	/// %m - minor version
	/// %r - revision
	/// %b - build
	/// all other characters are send directly to the output
	std::string toString( const char* format ) const;
};

} // namespace _Wolframe

#endif // _VERSION_HPP_INCLUDED
