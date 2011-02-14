//
// authentication_textfile.cpp
//

#include "authentication_textfile.hpp"

#include "unused.h"

namespace _SMERP {
	namespace Authentication {

TextFileAuthenticator::TextFileAuthenticator( SMERP_UNUSED const std::string _filename )
{
}

Step::AuthStep TextFileAuthenticator::nextStep( )
{
	return Step::_SMERP_AUTH_STEP_FAIL;
}

std::string TextFileAuthenticator::sendData( )
{
	return 0;
}

std::string TextFileAuthenticator::token( )
{
	return 0;
}

void TextFileAuthenticator::receiveData( SMERP_UNUSED const std::string data )
{
}

} // namespace Authentication
} // namespace _SMERP
