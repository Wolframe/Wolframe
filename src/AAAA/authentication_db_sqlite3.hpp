//
// authentication_db_sqlite3.hpp
//

#ifndef _AUTHENTICATION_DB_SQLITE3_HPP_INCLUDED
#define _AUTHENTICATION_DB_SQLITE3_HPP_INCLUDED

#include "AAAA/authentication.hpp"

#include <map>
#include <string>

#include "db/sdsqlite/sdsqlite.h"

namespace _Wolframe {
	namespace Authentication {

class DbSqlite3Authenticator : public Authenticator {
	private:
		std::map< std::string, std::string > m_creds;
		
		enum {
			_Wolframe_TEXTFILE_STATE_NEED_LOGIN,
			_Wolframe_TEXTFILE_STATE_NEED_PASS,
			_Wolframe_TEXTFILE_STATE_COMPUTE
		} m_state;

		sd::sqlite m_db;
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

} // namespace Authentication
} // namespace _Wolframe

#endif // _AUTHENTICATION_DB_SQLITE3_HPP_INCLUDED
