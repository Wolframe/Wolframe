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
/// \file version.hpp
/// \brief Header file for the Version class

#ifndef _VERSION_HPP_INCLUDED
#define _VERSION_HPP_INCLUDED

#include <string>

namespace _Wolframe {

/// version class
class Version {
private:
	unsigned short	m_major;		///< Major version.
	unsigned short	m_minor;		///< Minor version.
	unsigned short	m_revision;		///< Revision.
	bool		m_hasRevision;		///< Is revision present ?
	unsigned	m_build;		///< Build number.
	bool		m_hasBuild;		///< Is build number present ?
public:
	/// \brief Empty Version constructor.
	/// This will contruct a Version object having major and minor versions set to 0,
	/// no revision number and no build number.
	Version()
		: m_major( 0 ), m_minor( 0 ),
		  m_revision( 0 ), m_hasRevision( false ),
		  m_build( 0 ), m_hasBuild( false )
	{}
	Version( const Version& o)
		: m_major( o.m_major ), m_minor( o.m_minor ),
		  m_revision( o.m_revision ), m_hasRevision( o.m_hasRevision ),
		  m_build( o.m_build ), m_hasBuild( o.m_hasBuild )
	{}

	/// \brief	Parse a version string
	/// \param[in]	version the version string to parse
	/// \param[in]	format the format string of the version:
	/// \li		%M = Major version
	/// \li		%m = Minor version
	/// \li		%r = revision
	/// \li		%b = build number
	/// \li		%% = %
	/// \li		%| = Accepted end of version string (if the version string ends
	///		here, the input is accepted. If not parsing is continued with
	///		the rest of the format string as pattern)
	explicit Version( const char* version, const char* format="%M.%m%|.%r%|.%b");

	/// \brief	Build a version object from an unsigned long
	///		The format of the number is MMmmrrbbb
	Version( unsigned long version );
	Version( unsigned short M, unsigned short m );
	Version( unsigned short M, unsigned short m, unsigned short r );
	Version( unsigned short M, unsigned short m, unsigned short r, unsigned b );

	/// Returns the major number of the version.
	unsigned short Major() const		{ return m_major; }
	/// Returns the minor number of the version.
	unsigned short Minor() const		{ return m_minor; }
	/// Returns the revision number of the version.
	unsigned short Revision() const		{ return m_revision; }
	/// Returns the build number of the version.
	unsigned Build() const			{ return m_build; }

	bool operator == ( const Version &other ) const;
	bool operator != ( const Version &other ) const	{ return !( *this == other ); }

	bool operator >  ( const Version &other ) const;
	bool operator >= ( const Version &other ) const	{ return !( *this < other ); }
	bool operator <  ( const Version &other ) const	{ return  ( other > *this ); }
	bool operator <= ( const Version &other ) const	{ return !( *this > other ); }

	/// Test if the other version is compatible with this version
	/// using the usual OSS versioning schemas
	bool isCompatible( const Version &other ) const;

	/// \brief	Print the version in 'major.minor.revision.build' format.
	std::string toString() const;

	/// \brief	Print the version using custom format.
	/// \param	format	the format string
	std::string toString( const char* format ) const;

	/// \brief	Output the version as an unsigned long
	///		The format of the output is MMmmrrbbb
	/// \note	All numbers (major, minor...) will be cut to the 2 most significant
	///		decimal digits except for the build number which will be cut
	///		to 3 decimal digits
	unsigned long toNumber() const;
};

} // namespace _Wolframe

#endif // _VERSION_HPP_INCLUDED
