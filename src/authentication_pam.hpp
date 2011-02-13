#ifndef _SMERP_AUTHENTICATION_PAM_HPP_INCLUDED
#define _SMERP_AUTHENTICATION_PAM_HPP_INCLUDED

#include "authentication.hpp"

#ifdef WITH_PAM

extern "C" {
#include <security/pam_appl.h> 
}

namespace _SMERP {
	namespace Authentication {

// PAM

class PAMCredentials : public UsernamePasswordCredentials {
};

typedef struct {
	const PAMCredentials *cred;
	std::string errmsg;
	pam_handle_t *h;
} pam_appdata;

extern "C" const char *msg_style_to_str( int msg_style );
extern "C" void null_and_free(	int nmsg, struct pam_response *pr );
		
extern "C" int pam_conv_func(	int nmsg, const struct pam_message **msg,
				struct pam_response **reply, void *appdata_ptr );

class PAMAuthenticator : public Authenticator {
	private:
		std::string service;
		
	public:
		PAMAuthenticator( );
		virtual bool authenticate( const Credentials *cred );
};

} // namespace Authentication
} // namespace _SMERP

#endif // WITH_PAM

#endif /* ifndef SMERP__AUTH_PAM_H */
