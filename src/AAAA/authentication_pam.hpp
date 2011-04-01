//
// authentication_pam.hpp
//

#ifndef _Wolframe_AUTHENTICATION_PAM_HPP_INCLUDED
#define _Wolframe_AUTHENTICATION_PAM_HPP_INCLUDED

#include "AAAA/authentication.hpp"

#ifdef WITH_PAM

#include <string>

#include <security/pam_appl.h> 

namespace _Wolframe {
	namespace Authentication {


typedef struct {
	std::string login;
	bool has_pass;
	std::string pass;
	std::string errmsg;
	pam_handle_t *h;
} pam_appdata;

class PAMAuthenticator : public Authenticator {
	private:
		std::string m_service;

		// states of the authenticator state machine
		enum {
			_Wolframe_PAM_STATE_NEED_LOGIN,
			_Wolframe_PAM_STATE_HAS_LOGIN,
			_Wolframe_PAM_STATE_NEED_PASS,
			_Wolframe_PAM_STATE_HAS_PASS,
			_Wolframe_PAM_STATE_ERROR
		} m_state;
		
		pam_appdata m_appdata;
		std::string m_token;
		std::string m_data;
		std::string m_error;
		struct pam_conv m_conv;
		
	public:
		PAMAuthenticator( const std::string _service );
		virtual Step::AuthStep nextStep( );
		virtual std::string sendData( );
		virtual std::string token( );
		virtual void receiveData( const std::string data );
		virtual std::string getError( );
};

} // namespace Authentication
} // namespace _Wolframe

#endif // WITH_PAM

#endif // _Wolframe_AUTHENTICATION_PAM_HPP_INCLUDED
