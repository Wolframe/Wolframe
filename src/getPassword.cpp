//
// getPassword.cpp
//

#ifdef WITH_SSL

#include <string>
#include "acceptor.hpp"

namespace _SMERP {
	namespace Authentication {

#if !defined(_WIN32)

#include <unistd.h>
#include <libintl.h>

	std::string getPassword()
	{
		char	*pass;

		pass = getpass( gettext( "Enter your password:" ));

		return std::string( pass );
	}

#else // defined(_WIN32)

#include <tchar.h>
#include <stdlib.h>
#include <conio.h>  

	std::string getPassword()
	{
		std::string pass = "";
		std::cout << "Enter your password:";
		char ch = _getch( );
		while( ch != 13 ) {
			std::cout << "*";
			pass.push_back( ch );
			char ch = _getch( );
		}
		
		return pass;
	}

#endif // defined(_WIN32)


} // namespace Network
} // namespace _SMERP

#endif // WITH_SSL
