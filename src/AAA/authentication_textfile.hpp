//
// authentication_textfile.hpp
//

#ifndef _AUTHENTICATION_TEXTFILE_HPP_INCLUDED
#define _AUTHENTICATION_TEXTFILE_HPP_INCLUDED

#include "AAA/authentication.hpp"

#include <map>
#include <string>

namespace _Wolframe {
	namespace Authentication {

Authenticator *CreateTextFileAuthenticator( AuthenticatorFactory::properties props );

class TextFileAuthenticator : public Authenticator {
	private:
		std::map< std::string, std::string > m_creds;
		
		enum {
			_Wolframe_TEXTFILE_STATE_NEED_LOGIN,
			_Wolframe_TEXTFILE_STATE_NEED_PASS,
			_Wolframe_TEXTFILE_STATE_COMPUTE
		} m_state;
		
		std::string m_token;
		std::string m_login;
		std::string m_pass;
				
	public:
		TextFileAuthenticator( const std::string _filename );
		virtual Step::AuthStep nextStep( );
		virtual std::string sendData( );
		virtual std::string token( );
		virtual void receiveData( const std::string data );
		virtual std::string getError( );
};

} // namespace Authentication
} // namespace _Wolframe

#endif // _AUTHENTICATION_TEXTFILE_HPP_INCLUDED
