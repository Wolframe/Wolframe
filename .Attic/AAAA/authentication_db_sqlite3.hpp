//
// authentication_db_sqlite3.hpp
//

#ifndef _AUTHENTICATION_DB_SQLITE3_HPP_INCLUDED
#define _AUTHENTICATION_DB_SQLITE3_HPP_INCLUDED

#include "AAAA/authentication.hpp"

#include <map>
#include <string>

#ifdef WITH_SQLITE3

#include "sqlite3.h"

namespace _Wolframe {
namespace AAAA {

class DbSqlite3Authenticator : public Authenticator {
	private:
		std::map< std::string, std::string > m_creds;

		enum {
			_Wolframe_DB_SQLITE3_STATE_NEED_LOGIN,
			_Wolframe_DB_SQLITE3_STATE_NEED_PASS,
			_Wolframe_DB_SQLITE3_STATE_COMPUTE
		} m_state;

		sqlite3 *m_db;
		std::string m_filename;

		std::string m_token;
		std::string m_login;
		std::string m_pass;

	public:
		DbSqlite3Authenticator( const std::string _filename );
		virtual ~DbSqlite3Authenticator( );
		virtual Step::AuthStep nextStep( );
		virtual std::string sendData( );
		virtual std::string token( );
		virtual void receiveData( const std::string data );
		virtual std::string getError( );
};

}} // namespace _Wolframe::AAAA

#endif // WITH_SQLITE3

#endif // _AUTHENTICATION_DB_SQLITE3_HPP_INCLUDED
