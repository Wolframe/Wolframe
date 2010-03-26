//
// getPassword.cpp
//

#include <string>

#include "server.hpp"


namespace _SMERP {

#if !defined(_WIN32)

#include <unistd.h>
#include <libintl.h>

std::string server::getPassword()
{
	char	*pass;

	pass = getpass( gettext( "Enter your password:" ));

	return std::string( pass );
}

#else // defined(_WIN32)

std::string server::getPassword()
{
	return "bla bla";
}

#endif // defined(_WIN32)


} // namespace _SMERP

