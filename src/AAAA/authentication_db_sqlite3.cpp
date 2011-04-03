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
	int rc;

	m_filename = _filename;

	rc = sqlite3_open( m_filename.c_str( ), &m_db );
	if( rc != SQLITE_OK ) {
		std::ostringstream ss;
		ss	<< "sqlite3_open failed with database file " << m_filename << ": "
			<< sqlite3_errmsg( m_db );
		(void)sqlite3_close( m_db );
		throw std::runtime_error( ss.str( ) );
	}
	
	m_state = _Wolframe_TEXTFILE_STATE_NEED_LOGIN;
}

DbSqlite3Authenticator::~DbSqlite3Authenticator( )
{
	(void)sqlite3_close( m_db );
}

#if 0
// TODO: for this we should use a DB abstraction layer (like sqlitexx) or
// apr_db_sqlite3, or dbi, or ... :-)
#if SQLITE_VERSION_NUMBER >= 3005000
	rc = sqlite3_prepare_v2( db, "select * from a where b>=$1 and b<=$2",
		-1, &stmt, &ptr );
#else
	rc = sqlite3_prepare( db, "select * from a where b>=$1 and b<=$2",
		-1, &stmt, &ptr );
#endif
	if( rc != SQLITE_OK ) {
		fprintf( stderr, "Unable to prepare statement: %s\n", sqlite3_errmsg( db ) );
		(void)sqlite3_close( db );
		exit( 1 );
	}
	rc = sqlite3_bind_int( stmt, 1, 3 );
	if( rc != SQLITE_OK ) {
		fprintf( stderr, "Unable to bind first parameter: %s\n", sqlite3_errmsg( db ) );
		(void)sqlite3_finalize( stmt );
		(void)sqlite3_close( db );
		exit( 1 );
	}
	rc = sqlite3_bind_int( stmt, 2, 4 );
	if( rc != SQLITE_OK ) {
		fprintf( stderr, "Unable to bind second parameter: %s\n", sqlite3_errmsg( db ) );
		(void)sqlite3_finalize( stmt );
		(void)sqlite3_close( db );
		exit( 1 );
	}
	printf( "number of bind parameters: %d (MUST: 2)\n", sqlite3_bind_parameter_count( stmt ) );

	rc = sqlite3_step( stmt );
	while( rc == SQLITE_ROW ) {
		switch( rc ) {
			case SQLITE_ROW:
				printf( "nof columns: %d\n", sqlite3_data_count( stmt ) );
				for( i = 0; i < sqlite3_data_count( stmt ); i++ ) {
					const char *colname = sqlite3_column_name( stmt, i );
					int data_type = sqlite3_column_type( stmt, i );
					switch( data_type ) {
						case SQLITE_INTEGER: {
							int value = sqlite3_column_int( stmt, i );
							printf( "%s(%d) = %d\n", colname, data_type, value );
						}
						break;

						case SQLITE_TEXT: {
							const unsigned char *value = sqlite3_column_text( stmt, i );
							printf( "%s(%d) = %s\n", colname, data_type, value );
						}
						break;
					}
				}
				break;
	
			default:
				fprintf( stderr, "Unable to step through statement: %s\n", sqlite3_errmsg( db ) );
				(void)sqlite3_finalize( stmt );
				(void)sqlite3_close( db );
				exit( 1 );
		}
		rc = sqlite3_step( stmt );
	}

	/* finalize prepared statement */
	rc = sqlite3_finalize( stmt );
	if( rc != SQLITE_OK ) {
		fprintf( stderr, "Unable to finalize statement: %s\n", sqlite3_errmsg( db ) );
		(void)sqlite3_close( db );
		exit( 1 );
	}

#endif

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
