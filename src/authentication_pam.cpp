#include "authentication_pam.hpp"

// PAM

#ifdef WITH_PAM

extern "C" {
#include <string.h>
#include <stdlib.h>
}

#include <sstream>
#include <stdexcept>

namespace _SMERP {
	namespace Authentication {

PAMAuthenticator::PAMAuthenticator( const std::string _service )
	: m_service( _service )
{
}

const char *msg_style_to_str( int msg_style )
{
	switch( msg_style ) {
		case PAM_PROMPT_ECHO_OFF:	return "PAM_PROMPT_ECHO_OFF";
		case PAM_TEXT_INFO:		return "PAM_TEXT_INFO";
		case PAM_ERROR_MSG:		return "PAM_ERROR_MSG";
		case PAM_PROMPT_ECHO_ON:	return "PAM_PROMPT_ECHO_ON";
		default:			return "<unknown msg type>";
	}
}

void null_and_free( int nmsg, struct pam_response *pr )
{
	int i;
	struct pam_response *r = pr;

	if( pr == NULL ) return;

	for( i = 0; i < nmsg; i++, r++ ) {
		if( r->resp != NULL ) {
			memset( r->resp, 0, strlen( r->resp ) );
			free( r->resp );
			r->resp = NULL;
		}
	}
	free( pr );
}

int pam_conv_func(	int nmsg, const struct pam_message **msg,
			struct pam_response **reply, void *appdata_ptr )
{
	int i;
	const struct pam_message *m = *msg;
	struct pam_response *r;
	int rc;
	const char *login_prompt = NULL;
	union { const char *s; const void *v; } login_prompt_union;

	pam_appdata *appdata = (pam_appdata *)appdata_ptr;

	login_prompt_union.s = login_prompt;

// check sanity of the messages passed
	if( nmsg <= 0 || nmsg >= PAM_MAX_NUM_MSG ) {
		std::ostringstream ss;
		ss	<< "Bad number of messages in PAM conversion function: " << nmsg
			<< "must be between " << nmsg << " and " << PAM_MAX_NUM_MSG;
		appdata->errmsg = ss.str( );
		*reply = NULL;
		return PAM_CONV_ERR;
	}

// allocate return messages
	if( ( *reply = r = (struct pam_response *)calloc( nmsg, sizeof( struct pam_response ) ) ) == NULL ) {
		appdata->errmsg = "Unable to allocate memory for replies";
		return PAM_BUF_ERR;
	}

	for( i = 0; i < nmsg; i++ ) {
		if( msg[i]->msg == NULL ) {
			std::ostringstream ss;
			
			ss	<< "Bad message number " << i
				<< " of type " << msg_style_to_str( m->msg_style )
				<< " is not supposed to be NULL!";
			appdata->errmsg = ss.str( );
			goto error;
		}

// initalize response
		r->resp = NULL;
		r->resp_retcode = 0;

		switch( msg[i]->msg_style ) {
// Usually we get prompted for a password, this is not always true though.
			case PAM_PROMPT_ECHO_OFF:
				r->resp = strdup( appdata->cred->m_password.c_str( ) );
				if( r->resp == NULL ) {
					appdata->errmsg = "Unable to allocate memory for password answer";
					goto error;
				}
				break;

// Check against the PAM_USER_PROMPT to be sure we have a login request.
// Always recheck because the library could change the prompt any time
			case PAM_PROMPT_ECHO_ON:
				rc = pam_get_item( appdata->h, PAM_USER_PROMPT, &login_prompt_union.v );
				if( rc != PAM_SUCCESS ) {
					std::ostringstream ss;
					
					ss	<< "pam_get_item( PAM_USER_PROMPT) failed with error "
						<< pam_strerror( appdata->h, rc ) << "(" << rc << ")";
					appdata->errmsg = ss.str( );
					goto error;
				}
				if( strcmp( m->msg, login_prompt ) == 0 ) {
					r->resp = strdup( appdata->cred->m_userName.c_str( ) );
					if( r->resp == NULL ) {
						appdata->errmsg = "Unable to allocate memory for login answer";
						goto error;
					}
				}
				break;

// internal pam errors, infos (TODO: log later)
			case PAM_ERROR_MSG:
			case PAM_TEXT_INFO:
				break;

			default: {
				std::ostringstream ss;
				ss	<< "Unknown message " << i << " of type "
					<< msg_style_to_str( m->msg_style ) << ": "
					<< m->msg;
				appdata->errmsg = ss.str( );
				goto error;
			}

// next message and response
			m++; r++;
		}
	}

	return PAM_SUCCESS;

error:
	null_and_free( i, r );
	*reply = NULL;
	return PAM_CONV_ERR;
}

bool PAMAuthenticator::authenticate( const Credentials *cred )
{
	int rc;
	pam_appdata appdata;
	struct pam_conv conv;
	pam_handle_t *h;

	appdata.cred = dynamic_cast<const PAMCredentials *>( cred );
	appdata.h = NULL;

	conv.conv = pam_conv_func;
	conv.appdata_ptr = &appdata;

// the service name must be a CONSTANT due to security reasons!
	rc = pam_start( m_service.c_str( ), appdata.cred->m_userName.c_str( ), &conv, &h );
	if( rc != PAM_SUCCESS ) {
		std::ostringstream ss;
		ss	<< "pam_start failed with service " << m_service << ": "
			<< pam_strerror( h, rc ) << " (" << rc << ")";
		throw std::runtime_error( ss.str( ) );
	}
	appdata.h = h;
		
// authenticate: are we who we claim to be?
	rc = pam_authenticate( h, 0 );
	if( rc != PAM_SUCCESS ) {
		std::ostringstream ss;
		ss	<< "pam_authenticate failed with service " << m_service << ": "
			<< pam_strerror( h, rc ) << " (" << rc << ")";
		(void)pam_end( h, rc );
		throw std::runtime_error( ss.str( ) );
	}

// is access permitted?
	rc = pam_acct_mgmt( h, 0 );
	if( rc != PAM_SUCCESS ) {
		std::ostringstream ss;
		ss	<< "pam_acct_mgmt failed with service " << m_service << ": "
			<< pam_strerror( h, rc ) << " (" << rc << ")";
		(void)pam_end( h, rc );
		throw std::runtime_error( ss.str( ) );
	}

// terminate PAM session with last exit code
	if( pam_end( h, rc ) != PAM_SUCCESS ) {
		std::ostringstream ss;
		ss	<< "pam_end failed with service " << m_service << ": "
			<< pam_strerror( h, rc ) << " (" << rc << ")";
		(void)pam_end( h, rc );
		throw std::runtime_error( ss.str( ) );
	}

	return ( rc == PAM_SUCCESS );
}

} // namespace Authentication
} // namespace _SMERP

#endif // WITH_PAM
