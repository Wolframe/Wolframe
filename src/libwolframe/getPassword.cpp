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
// getPassword.cpp
//
#include "utils/getPassword.hpp"
#include <string>
#if !defined(_WIN32)
#include <unistd.h>
#include <libintl.h>
#else // defined(_WIN32)
#include <tchar.h>
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <stdlib.h>
#include <conio.h>
#include <cstdio>
#endif // defined(_WIN32)

#if !defined(_WIN32)
	std::string _Wolframe::AAAA::getLogin( )
	{
		return getlogin( );
	}

	std::string _Wolframe::AAAA::getPassword()
	{
		char	*pass;

		pass = getpass( gettext( "Enter your password:" ));

		return std::string( pass );
	}

#else // defined(_WIN32)
	std::string _Wolframe::AAAA::getLogin( )
	{
		TCHAR login[256];
		DWORD len = 254;
		GetUserName( login, &len );
		return std::string( login );
	}

	std::string _Wolframe::AAAA::getPassword()
	{
		std::string pass = "";

		_cputs( "Enter your password:" );

		int ch = _getch( );
		while( ch != 13 ) {
			pass.push_back( ch );
			ch = _getch( );
		}
		puts( "" );

		return pass;
	}
#endif // defined(_WIN32)

