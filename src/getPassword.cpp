//
// getPassword.cpp
//

#include <string>

#include "acceptor.hpp"


namespace _SMERP {

#if !defined(_WIN32)

#include <unistd.h>
#include <libintl.h>

std::string SSLacceptor::getPassword()
{
	char	*pass;

	pass = getpass( gettext( "Enter your password:" ));

	return std::string( pass );
}

#else // defined(_WIN32)

std::string SSLacceptor::getPassword()
{
	return "bla bla";
}

#endif // defined(_WIN32)


} // namespace _SMERP

