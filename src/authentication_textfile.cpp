//
// authentication_textfile.cpp
//

#include "authentication_textfile.hpp"

#include <stdexcept>

#include "unused.h"
#include <utility>

namespace _SMERP {
	namespace Authentication {

Authenticator *CreateTextFileAuthenticator( AuthenticatorFactory::properties props )
{
	return new TextFileAuthenticator(
		findprop<std::string>( props, "filename" )
	);
}

TextFileAuthenticator::TextFileAuthenticator( SMERP_UNUSED const std::string _filename )
{
	m_state = _SMERP_TEXTFILE_STATE_NEED_LOGIN;

	// TODO: read file here
	m_creds.insert( std::make_pair<std::string, std::string>( "abaumann", "xx" ) );
}

Step::AuthStep TextFileAuthenticator::nextStep( )
{
	switch( m_state ) {
		case _SMERP_TEXTFILE_STATE_NEED_LOGIN:
			m_token = "login";
			return Step::_SMERP_AUTH_STEP_RECV_DATA;
		
		case _SMERP_TEXTFILE_STATE_NEED_PASS:
			m_token = "password";
			return Step::_SMERP_AUTH_STEP_RECV_DATA;
			
		case _SMERP_TEXTFILE_STATE_COMPUTE:
			std::map<std::string, std::string>::const_iterator it = m_creds.find( m_login );
// user not in text file
			if( it == m_creds.end( ) ) {
				m_state = _SMERP_TEXTFILE_STATE_NEED_LOGIN;
				return Step::_SMERP_AUTH_STEP_FAIL;
			}
// user found, but password doesn't match			
			if( it->second != m_pass ) {
				m_state = _SMERP_TEXTFILE_STATE_NEED_LOGIN;
				return Step::_SMERP_AUTH_STEP_FAIL;			
			}
			
// everythink is peachy
			m_state = _SMERP_TEXTFILE_STATE_NEED_LOGIN;
			return Step::_SMERP_AUTH_STEP_SUCCESS;
	}

	return Step::_SMERP_AUTH_STEP_FAIL;
}

// never used
std::string TextFileAuthenticator::sendData( )
{
	return 0;
}

std::string TextFileAuthenticator::token( )
{
	return m_token;
}

void TextFileAuthenticator::receiveData( SMERP_UNUSED const std::string data )
{
	switch( m_state ) {
		case _SMERP_TEXTFILE_STATE_NEED_LOGIN:
			m_login = data;
			m_state = _SMERP_TEXTFILE_STATE_NEED_PASS;
			break;
		
		case _SMERP_TEXTFILE_STATE_NEED_PASS:
			m_pass = data;
			m_state = _SMERP_TEXTFILE_STATE_COMPUTE;
			break;

// TODO: application exception		
		case _SMERP_TEXTFILE_STATE_COMPUTE:
			throw new std::runtime_error( "Illegal state in auhenticator" );
			break;
	}
}

std::string TextFileAuthenticator::getError( )
{
	return "";
}

} // namespace Authentication
} // namespace _SMERP
