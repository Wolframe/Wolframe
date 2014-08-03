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
/// \file  platform.cpp
/// \brief Platform class implementation

#include "platform.hpp"

#include <sstream>

#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif

namespace _Wolframe {

std::string Platform::toString( ) const
{
	std::stringstream ss;
	
	ss << m_os; 
	
	if( m_hasOsVersion ) {
		ss << " " << m_os_major << "." << m_os_minor;
	}
	
	if( m_hasDisto ) {
		ss << ", " << m_distro;
		if( m_hasDistroVersion ) {
			ss << " " << m_distro_major << "." << m_distro_minor;
		}
	}

	return ss.str( );
}

Platform Platform::makePlatform( )
{
	Platform p;

#ifdef _WIN32
	p.m_os = "Windows";
	// TODO: later, check _WIN32_WINNT
	p.m_hasOsVersion = false;
#endif // _WIN32
#ifdef LINUX
	p.m_os = "Linux";
	p.m_hasOsVersion = true;
	p.m_os_major = OS_MAJOR_VERSION;
	p.m_os_minor = OS_MINOR_VERSION;
#ifdef LINUX_DIST_ARCH
	p.m_hasDisto = true;
	p.m_distro = "Archlinux";
	p.m_hasDistroVersion = false;
#endif // LINUX_DIST_ARCH
#ifdef LINUX_DIST_DEBIAN
	p.m_hasDisto = true;
	p.m_distro = "Debian";
	p.m_hasDistroVersion = true;
#endif // LINUX_DIST_DEBIAN
#ifdef LINUX_DIST_REDHAT
	p.m_hasDisto = true;
	p.m_distro = "Redhat";
	p.m_hasDistroVersion = true;
#endif // LINUX_DIST_REDHAT
#ifdef LINUX_DIST_SLACKWARE
	p.m_hasDisto = true;
	p.m_distro = "Slackware";
	p.m_hasDistroVersion = true;
#endif // LINUX_DIST_SLACKWARE
#ifdef LINUX_DIST_SLES
	p.m_hasDisto = true;
	p.m_distro = "SLES";
	p.m_hasDistroVersion = true;
#endif // LINUX_DIST_SLES
#ifdef LINUX_DIST_SUSE
	p.m_hasDisto = true;
	p.m_distro = "openSuSE";
	p.m_hasDistroVersion = true;
#endif // LINUX_DIST_SUSE
#ifdef LINUX_DIST_UBUNTU
	p.m_hasDisto = true;
	p.m_distro = "Ubuntu";
	p.m_hasDistroVersion = true;
#endif // LINUX_DIST_UBUNTU
#endif // LINUX
#ifdef SUNOS
	p.m_os = "SunOS";
	p.m_hasOsVersion = true;
	p.m_os_major = OS_MAJOR_VERSION;
	p.m_os_minor = OS_MINOR_VERSION;
	p.m_hasDistroVersion = false;
#endif // SUNOS
#ifdef FREEBSD
	p.m_os = "FreeBSD";
	p.m_hasOsVersion = true;
	p.m_os_major = OS_MAJOR_VERSION;
	p.m_os_minor = OS_MINOR_VERSION;
	p.m_hasDistroVersion = false;
#endif // FREEBSD
#ifdef NETBSD
	p.m_os = "NetBSD";
	p.m_hasOsVersion = true;
	p.m_os_major = OS_MAJOR_VERSION;
	p.m_os_minor = OS_MINOR_VERSION;
	p.m_hasDistroVersion = false;
#endif // NETBSD

	return p;
}

Platform Platform::runtimePlatform( )
{
	Platform p;
	
	p.m_os = "Not Implemented";
	return p;
}

} // namespace _Wolframe
