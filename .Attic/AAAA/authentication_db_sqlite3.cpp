//
// authentication_db_sqlite3.cpp
//

// TODO: compare hashes, add a cram

#include "AAAA/authentication_db_sqlite3.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>

#include <stdexcept>
#include <fstream>

namespace _Wolframe {
namespace AAAA {

DbSqlite3Authenticator::DbSqlite3Authenticator( const std::string _filename )
{
	m_filename = _filename;

	int res = sqlite3_open( m_filename.c_str( ), &m_db );
	if( res != SQLITE_OK ) {
		std::ostringstream oss;
		oss << "Unable to open Sqlite3 database '" << m_filename << ": " << sqlite3_errmsg( m_db );
		throw std::runtime_error( oss.str( ) );
	}

	m_state = _Wolframe_DB_SQLITE3_STATE_NEED_LOGIN;
}

DbSqlite3Authenticator::~DbSqlite3Authenticator( )
{
	sqlite3_close( m_db );
}

Step::AuthStep DbSqlite3Authenticator::nextStep( )
{
	int rc;
	sqlite3_stmt *stmt;
	std::string sql;
	const char *tail;
	const char *pass = NULL;

	switch( m_state ) {
		case _Wolframe_DB_SQLITE3_STATE_NEED_LOGIN:
			m_token = "login";
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;

		case _Wolframe_DB_SQLITE3_STATE_NEED_PASS:
			// TODO: cram, not password in plain!
			m_token = "password";
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;

		case _Wolframe_DB_SQLITE3_STATE_COMPUTE:
			sql = "select password from users where login=?";
// check if user is in the sqlite table
#if SQLITE_VERSION_NUMBER >= 3005000
			rc = sqlite3_prepare_v2( m_db, sql.c_str( ), -1, &stmt, &tail );
#else
			rc = sqlite3_prepare( m_db, sql.c_str( ), -1, &stmt, &tail );
#endif
			if( rc != SQLITE_OK ) {
				std::ostringstream oss;
				oss << "Unable to prepare SQL statement '" << sql << ": " << sqlite3_errmsg( m_db );
				throw std::runtime_error( oss.str( ) );
			}

			rc = sqlite3_bind_text( stmt, 1, m_login.c_str( ), m_login.length( ), SQLITE_STATIC );
			if( rc != SQLITE_OK ) {
				std::ostringstream oss;
				oss << "Unable to bind parameter login in '" << sql << ": " << sqlite3_errmsg( m_db );
				throw std::runtime_error( oss.str( ) );
			}

			rc = sqlite3_step( stmt );
			if( rc == SQLITE_DONE ) {
				m_state = _Wolframe_DB_SQLITE3_STATE_NEED_LOGIN;
				goto FAIL;
			} else if( rc == SQLITE_ROW ) {
				pass = (const char *)sqlite3_column_text( stmt, 0 );
			}

// user found, but password doesn't match
			if( strcmp( pass, m_pass.c_str( ) ) != 0 ) {
				m_state = _Wolframe_DB_SQLITE3_STATE_NEED_LOGIN;
				goto FAIL;
			}

			sqlite3_finalize( stmt );

// everythink is peachy
			m_state = _Wolframe_DB_SQLITE3_STATE_NEED_LOGIN;
			return Step::_Wolframe_AUTH_STEP_SUCCESS;
	}

FAIL:
	sqlite3_finalize( stmt );

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
		case _Wolframe_DB_SQLITE3_STATE_NEED_LOGIN:
			m_login = data;
			m_state = _Wolframe_DB_SQLITE3_STATE_NEED_PASS;
			break;

		case _Wolframe_DB_SQLITE3_STATE_NEED_PASS:
			m_pass = data;
			m_state = _Wolframe_DB_SQLITE3_STATE_COMPUTE;
			break;

// TODO: application exception
		case _Wolframe_DB_SQLITE3_STATE_COMPUTE:
			throw new std::runtime_error( "Illegal state in auhenticator" );
			break;
	}
}

std::string DbSqlite3Authenticator::getError( )
{
	return "";
}

}} // namespace _Wolframe::AAAA
