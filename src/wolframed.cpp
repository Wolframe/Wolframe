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
/// \file wolframed.cpp

#include "appProperties.hpp"
#include "wolframe.hpp"
#include "version.hpp"

class WolframeAppProperties
	:public _Wolframe::AppProperties
{
public:
	virtual const char* applicationName() const		{ return "Wolframe"; }
	virtual const _Wolframe::Version applicationVersion() const	{ return _Wolframe::Version(
										  WOLFRAME_MAJOR_VERSION,
										  WOLFRAME_MINOR_VERSION,
										  WOLFRAME_REVISION
										  ); }

	virtual const char* defaultMainConfig() const		{ return "/etc/wolframe.conf"; }
	virtual const char* defaultUserConfig() const		{ return "~/wolframe.conf"; }
	virtual const char* defaultLocalConfig() const		{ return "./wolframe.conf"; }

	virtual unsigned short defaultTCPport() const		{ return 7660; }
	virtual unsigned short defaultSSLport() const		{ return 7960; }

	virtual const char* defaultServiceName() const		{ return "wolframe"; }
#if defined( _WIN32 )
	const char* defaultServiceDisplayName() const		{ return "Wolframe Daemon"; }
	const char* defaultServiceDescription() const		{ return "a daemon for wolframeing"; }
#endif // defined( _WIN32 )
};

static WolframeAppProperties g_appProperties;


#if defined(_WIN32)		// we are on Windows

	int _Wolframe_winMain( int argc, char* argv[], const _Wolframe::AppProperties* appProperties);

	int main( int argc, char* argv[] )
	{
		return ( _Wolframe_winMain( argc, argv, &g_appProperties ));
	}

#else			// we are on a POSIX system

	int _Wolframe_posixMain( int argc, char* argv[], const _Wolframe::AppProperties* appProperties );

	int main( int argc, char* argv[] )
	{
		return( _Wolframe_posixMain( argc, argv, &g_appProperties ));
	}
#endif // !defined(_WIN32)

