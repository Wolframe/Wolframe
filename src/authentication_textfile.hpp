//
// authentication_textfile.hpp
//

#ifndef _AUTHENTICATION_TEXTFILE_HPP_INCLUDED
#define _AUTHENTICATION_TEXTFILE_HPP_INCLUDED

#include "authentication.hpp"

namespace _SMERP {
	namespace Authentication {

class TextFileAuthenticator : public Authenticator {		
	public:
		TextFileAuthenticator( const std::string _filename );
		virtual Step::AuthStep nextStep( ) = 0;
		virtual std::string sendData( );
		virtual std::string token( );
		virtual void receiveData( const std::string data );
};

} // namespace Authentication
} // namespace _SMERP

#endif // _AUTHENTICATION_TEXTFILE_HPP_INCLUDED