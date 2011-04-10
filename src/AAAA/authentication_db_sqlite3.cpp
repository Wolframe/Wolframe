//
// authentication_db_sqlite3.cpp
//

#include "AAAA/authentication_db_sqlite3.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>

#include <stdexcept>
#include <fstream>

namespace _Wolframe {
	namespace Authentication {

Authenticator *CreateDbSqlite3Authenticator( AuthenticatorFactory::properties props )
{
	return new DbSqlite3Authenticator(
		findprop<std::string>( props, "filename" )
	);
}

DbSqlite3Authenticator::DbSqlite3Authenticator( const std::string _filename )
{
	m_filename = _filename;

	m_db.open( m_filename );
	
	m_state = _Wolframe_TEXTFILE_STATE_NEED_LOGIN;
}

DbSqlite3Authenticator::~DbSqlite3Authenticator( )
{
	m_db.close( );
}

Step::AuthStep DbSqlite3Authenticator::nextStep( )
{
	switch( m_state ) {
		case _Wolframe_TEXTFILE_STATE_NEED_LOGIN:
			m_token = "login";
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;
		
		case _Wolframe_TEXTFILE_STATE_NEED_PASS:
			// TODO: cram, not password in plain!
			m_token = "password";
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;
			
		case _Wolframe_TEXTFILE_STATE_COMPUTE:
// check if user is in the sqlite table
// TODO: select from users where login=XXXX		
//			if( it == m_creds.end( ) ) {
//				m_state = _Wolframe_TEXTFILE_STATE_NEED_LOGIN;
//				goto FAIL;
//			}
// user found, but password doesn't match
// TODO: compare hashes, add a cram
//			if( it->second != m_pass ) {
//				m_state = _Wolframe_TEXTFILE_STATE_NEED_LOGIN;
//				goto FAIL;
//			}
			
// everythink is peachy
			m_state = _Wolframe_TEXTFILE_STATE_NEED_LOGIN;
			return Step::_Wolframe_AUTH_STEP_SUCCESS;
	}

//FAIL:
	boost::this_thread::sleep( boost::posix_time::seconds( 1 ) );

	return Step::_Wolframe_AUTH_STEP_FAIL;
}

// never used
std::string DbSqlite3Authenticator::sendData( )
{
	return 0;
}

std::string DbSqlite3Authenticator::token( )
{
	return m_token;
}

void DbSqlite3Authenticator::receiveData( const std::string data )
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

std::string DbSqlite3Authenticator::getError( )
{
	return "";
}

} // namespace Authentication
} // namespace _Wolframe
