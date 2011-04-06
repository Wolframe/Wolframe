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
// application properties - implementation
//

#include "appProperties.hpp"

namespace _Wolframe	{

	const char*	applicationName()				{ return "Wolframe"; }
	unsigned short	applicationMajorVersion()			{ return 0; }
	unsigned short	applicationMinorVersion()			{ return 0; }
	unsigned short	applicationRevisionVersion()			{ return 4; }
	unsigned	applicationBuildVersion()			{ return 0; }

	const char*	Configuration::defaultMainConfig()		{ return "/etc/wolframe.conf"; }
	const char*	Configuration::defaultUserConfig()		{ return "~/wolframe.conf"; }
	const char*	Configuration::defaultLocalConfig()		{ return "./wolframe.conf"; }

	unsigned short	Configuration::defaultTCPport()			{ return 7660; }
	unsigned short	Configuration::defaultSSLport()			{ return 7960; }

	const char*	Configuration::defaultServiceName()		{ return "wolframe"; }
#if defined( _WIN32 )
	const char*	Configuration::defaultServiceDisplayName()	{ return "Wolframe Daemon"; }
	const char*	Configuration::defaultServiceDescription()	{ return "a daemon for wolframeing"; }
#endif // defined( _WIN32 )

} // namespace _Wolframe

