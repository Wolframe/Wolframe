#ifndef _SMERP_AUTHENTICATION_PAM_HPP_INCLUDED
#define _SMERP_AUTHENTICATION_PAM_HPP_INCLUDED

#include "authentication.hpp"

#ifdef WITH_PAM

extern "C" {
#include <security/pam_appl.h> 
}

namespace _SMERP {
	namespace Authentication {


typedef struct {
	std::string login;
	std::string pass;
	std::string errmsg;
	pam_handle_t *h;
} pam_appdata;

extern "C" const char *msg_style_to_str( int msg_style );
extern "C" void null_and_free(	int nmsg, struct pam_response *pr );
		
extern "C" int pam_conv_func(	int nmsg, const struct pam_message **msg,
				struct pam_response **reply, void *appdata_ptr );

class PAMAuthenticator : public Authenticator {
	private:
		std::string m_service;
		
	public:
		PAMAuthenticator( const std::string _service );
		virtual Step::AuthStep nextStep( );
		virtual std::string sendData( );
		virtual std::string token( );
		virtual void receiveData( const std::string data );
};

} // namespace Authentication
} // namespace _SMERP

#endif // WITH_PAM

#endif // _SMERP_AUTHENTICATION_PAM_HPP_INCLUDED
