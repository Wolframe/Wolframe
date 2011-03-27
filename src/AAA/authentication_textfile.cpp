//
// authentication_textfile.cpp
//

#include "AAA/authentication_textfile.hpp"

#include <boost/algorithm/string.hpp>

#include <stdexcept>
#include <fstream>

namespace _Wolframe {
	namespace Authentication {

Authenticator *CreateTextFileAuthenticator( AuthenticatorFactory::properties props )
{
	return new TextFileAuthenticator(
		findprop<std::string>( props, "filename" )
	);
}

TextFileAuthenticator::TextFileAuthenticator( const std::string _filename )
{
	std::ifstream f;
	
	f.open( _filename.c_str( ), std::ifstream::in );
	if( f.good( ) ) {
		while( !f.eof( ) ) {
			char line[256];
			f.getline( line, 255 );
			std::vector<std::string> v;
			split( v, line, boost::is_any_of( "\t" ) );
			if( v.size( ) == 2 ) {
				m_creds.insert( std::make_pair<std::string, std::string>( v[0], v[1] ) );
			}
		}
		f.close( );
	}
	
	m_state = _Wolframe_TEXTFILE_STATE_NEED_LOGIN;
}

Step::AuthStep TextFileAuthenticator::nextStep( )
{
	switch( m_state ) {
		case _Wolframe_TEXTFILE_STATE_NEED_LOGIN:
			m_token = "login";
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;
		
		case _Wolframe_TEXTFILE_STATE_NEED_PASS:
			m_token = "password";
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;
			
		case _Wolframe_TEXTFILE_STATE_COMPUTE:
			std::map<std::string, std::string>::const_iterator it = m_creds.find( m_login );
// user not in text file
			if( it == m_creds.end( ) ) {
				m_state = _Wolframe_TEXTFILE_STATE_NEED_LOGIN;
				return Step::_Wolframe_AUTH_STEP_FAIL;
			}
// user found, but password doesn't match			
			if( it->second != m_pass ) {
				m_state = _Wolframe_TEXTFILE_STATE_NEED_LOGIN;
				return Step::_Wolframe_AUTH_STEP_FAIL;			
			}
			
// everythink is peachy
			m_state = _Wolframe_TEXTFILE_STATE_NEED_LOGIN;
			return Step::_Wolframe_AUTH_STEP_SUCCESS;
	}

	return Step::_Wolframe_AUTH_STEP_FAIL;
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

void TextFileAuthenticator::receiveData( const std::string data )
{
	switch( m_state ) {
		case _Wolframe_TEXTFILE_STATE_NEED_LOGIN:
			m_login = data;
			m_state = _Wolframe_TEXTFILE_STATE_NEED_PASS;
			break;
		
		case _Wolframe_TEXTFILE_STATE_NEED_PASS:
			m_pass = data;
			m_state = _Wolframe_TEXTFILE_STATE_COMPUTE;
			break;

// TODO: application exception		
		case _Wolframe_TEXTFILE_STATE_COMPUTE:
			throw new std::runtime_error( "Illegal state in auhenticator" );
			break;
	}
}

std::string TextFileAuthenticator::getError( )
{
	return "";
}

} // namespace Authentication
} // namespace _Wolframe
