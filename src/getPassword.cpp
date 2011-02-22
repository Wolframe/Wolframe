//
// getPassword.cpp
//

#include <string>

namespace _SMERP {
	namespace Authentication {

#if !defined(_WIN32)

#include <unistd.h>
#include <libintl.h>

	std::string getLogin( )
	{
		return getlogin( );
	}

	std::string getPassword()
	{
		char	*pass;

		pass = getpass( gettext( "Enter your password:" ));

		return std::string( pass );
	}

#else // defined(_WIN32)

#include <tchar.h>
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <stdlib.h>
#include <conio.h>
#include <cstdio>

	std::string getLogin( )
	{
		TCHAR login[256];
		DWORD len = 254;
		GetUserName( login, &len );
		return std::string( login );
	}
	
	std::string getPassword()
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


} // namespace Authentication
} // namespace _SMERP
