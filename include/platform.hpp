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
/// \file platform.hpp
/// \brief Header file for the Platform class

#ifndef _PLATFORM_HPP_INCLUDED
#define _PLATFORM_HPP_INCLUDED

#include <string>

namespace _Wolframe {

/// platform class
class Platform {
private:
	std::string	m_os;			///< Operating system
	bool		m_hasOsVersion;		///< OS has a version?
	unsigned short	m_os_major;		///< Operating system major version.
	unsigned short	m_os_minor;		///< Operating system minor version.
	bool		m_hasDisto;		///< Is it a Linux distribution?
	std::string	m_distro;		///< Name of the Linux distribution
	bool		m_hasDistroVersion;	///< Distribution has a version?
	unsigned short	m_distro_major;		///< Linux distribution major version.
	unsigned short	m_distro_minor;		///< Linux distribution minor version.	

public:
	/// \brief Empty Version constructor.
	/// This will contruct an empty Platform object having version
	/// numbers set to 0 and string be empty
	Platform()
		: m_os( "" ), m_hasOsVersion( false ),
		  m_os_major( 0 ), m_os_minor( 0 ),
		  m_hasDisto( false ),
		  m_distro( "" ), 
		  m_hasDistroVersion( false ),
		  m_distro_major( 0 ), m_distro_minor( 0 )
	{}
	Platform( const Platform& o)
		: m_os( o.m_os), m_hasOsVersion( o.m_hasOsVersion ),
		  m_os_major( o.m_os_major ), m_os_minor( o.m_os_minor ),
		  m_hasDisto( o.m_hasDisto ),
		  m_distro( o.m_distro ),
		  m_hasDistroVersion( o.m_hasDistroVersion ),
		  m_distro_major( o.m_distro_major ), m_distro_minor( o.m_distro_minor )
	{}

	/// \brief	Print the plaform in human readable format.
	std::string toString() const;
	
	/// \brief	Create platform from makefile system.
	///		This is the platform set for building the software
	///		not where the software was build on nor the where it
	///		is run on (think cross-compilation and compatible
	///		distributions)
	static Platform makePlatform( );
	
	/// \brief	Create plaform as seen at runtime.
	///		This function uses functions to determine the runtime
	///		platform we are running on, not the platform we built
	///		the software for.
	static Platform runtimePlatform( );
};

} // namespace _Wolframe

#endif // _PLATFORM_HPP_INCLUDED
