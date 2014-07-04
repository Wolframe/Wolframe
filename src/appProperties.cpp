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
//
// application properties - implementation
//

#include "appProperties.hpp"
#include "wolframe.hpp"
#include "version.hpp"

namespace _Wolframe	{

	const char*	applicationName()			{ return "Wolframe"; }
	const Version	applicationVersion()			{ return Version( WOLFRAME_MAJOR_VERSION,
										  WOLFRAME_MINOR_VERSION,
										  WOLFRAME_REVISION
										  ); }

	const char*	config::defaultMainConfig()		{ return "/etc/wolframe.conf"; }
	const char*	config::defaultUserConfig()		{ return "~/wolframe.conf"; }
	const char*	config::defaultLocalConfig()		{ return "./wolframe.conf"; }

	unsigned short	net::defaultTCPport()			{ return 7661; }
	unsigned short	net::defaultSSLport()			{ return 7961; }

#if defined( _WIN32 )
	const char*	config::defaultServiceName()		{ return "wolframe"; }
	const char*	config::defaultServiceDisplayName()	{ return "Wolframe Daemon"; }
	const char*	config::defaultServiceDescription()	{ return "a daemon for wolframeing"; }
#endif // defined( _WIN32 )

} // namespace _Wolframe

