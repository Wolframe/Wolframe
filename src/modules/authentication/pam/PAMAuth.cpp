/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//
//
//

#include "PAMAuth.hpp"

#include "logger-v1.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <sstream>

namespace _Wolframe {
namespace AAAA {

/// Text file authentication
bool PAMAuthConfig::check() const
{
	if ( m_service.empty() )	{
		LOG_ERROR << logPrefix() << "PAM authentication service cannot be empty";
		return false;
	}
	return true;
}

void PAMAuthConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );
	os << indStr << sectionName() << std::endl;
	os << indStr << "   Identifier: " << m_identifier << std::endl;
	os << indStr << "   Service: " << m_service << std::endl;
}

// Solaris defines the pam client callback function slightly different
#ifdef SUNOS
static int pam_conv_func(	int nmsg, struct pam_message **msg,
				struct pam_response **reply, void *appdata_ptr );
#else
static int pam_conv_func(	int nmsg, const struct pam_message **msg,
				struct pam_response **reply, void *appdata_ptr );
#endif

static const char *msg_style_to_str( int msg_style )
{
	switch( msg_style ) {
		case PAM_PROMPT_ECHO_OFF:	return "PAM_PROMPT_ECHO_OFF";
		case PAM_TEXT_INFO:		return "PAM_TEXT_INFO";
		case PAM_ERROR_MSG:		return "PAM_ERROR_MSG";
		case PAM_PROMPT_ECHO_ON:	return "PAM_PROMPT_ECHO_ON";
		default:			return "<unknown msg type>";
	}
}

static void null_and_free( int nmsg, struct pam_response *pr )
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

#ifdef SUNOS
static int pam_conv_func(	int nmsg, struct pam_message **msg,
				struct pam_response **reply, void *appdata_ptr )
#else
static int pam_conv_func(	int nmsg, const struct pam_message **msg,
				struct pam_response **reply, void *appdata_ptr )
#endif
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
		*reply = NULL;
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
// thank you very much, come again (but with a password)
				if( !appdata->has_pass ) {
					*reply = NULL;
// Solaris and NetBSD have no PAM_CONV_AGAIN, returning an error instead
#if defined SUNOS || NETBSD
					return PAM_CONV_ERR;
#else
					return PAM_CONV_AGAIN;
#endif
				}
				r->resp = strdup( appdata->pass.c_str( ) );
				if( r->resp == NULL ) {
					appdata->errmsg = "Unable to allocate memory for password answer";
					goto error;
				}
				break;

// Check against the PAM_USER_PROMPT to be sure we have a login request.
// Always recheck because the library could change the prompt any time
			case PAM_PROMPT_ECHO_ON:
// Solaris has a different API in pam_get_item
#ifdef SUNOS
				rc = pam_get_item( appdata->h, PAM_USER_PROMPT, (void **)&login_prompt_union.v );
#else
				rc = pam_get_item( appdata->h, PAM_USER_PROMPT, &login_prompt_union.v );
#endif
				if( rc != PAM_SUCCESS ) {
					std::ostringstream ss;

					ss	<< "pam_get_item( PAM_USER_PROMPT) failed with error "
						<< pam_strerror( appdata->h, rc ) << "(" << rc << ")";
					appdata->errmsg = ss.str( );
					goto error;
				}
				if( login_prompt == NULL || strcmp( m->msg, login_prompt ) == 0 ) {
// thank you very much, come again (but with a username)
					if( !appdata->has_login ) {
						*reply = NULL;
// Solaris and NetBSD have no PAM_CONV_AGAIN, returning an error instead
#if defined SUNOS || NETBSD
						return PAM_CONV_ERR;
#else
						return PAM_CONV_AGAIN;
#endif
					} else {
						r->resp = strdup( appdata->login.c_str( ) );
						if( r->resp == NULL ) {
							appdata->errmsg = "Unable to allocate memory for login answer";
							goto error;
						}
					}
				}
				break;

// internal pam errors, infos (TODO: log later)
			case PAM_ERROR_MSG:
			case PAM_TEXT_INFO:
				appdata->errmsg = m->msg;
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

//*********   PAM Authentication Unit   *************************************

PAMAuthUnit::PAMAuthUnit( const std::string& Identifier,
				    const std::string& service )
	: AuthenticationUnit( Identifier ), m_service( service )
{
	LOG_DEBUG << "PAM authentication unit created with PAM service '" << m_service << "'";
}

PAMAuthUnit::~PAMAuthUnit()
{
}

const char** PAMAuthUnit::mechs() const
{
	static const char* m[] = { "WOLFRAME-PAM", NULL };
	return m;
}

AuthenticatorSlice* PAMAuthUnit::slice( const std::string& /*mech*/,
					const net::RemoteEndpoint& /*client*/ )
{
	return new PAMAuthSlice( *this );
}

User* PAMAuthUnit::authenticatePlain(	const std::string& username,
					const std::string& password ) const
{
	int rc;
	pam_appdata appdata;
	const struct pam_conv conv = { pam_conv_func, &appdata };

	appdata.has_login = true;
	appdata.login = username;
	appdata.h = NULL;
	appdata.has_pass = true;
	appdata.pass = password;

	rc = pam_start( m_service.c_str( ), NULL, &conv, &appdata.h );
	if( rc != PAM_SUCCESS ) {
		LOG_ERROR << "pam_start failed with service '" << m_service << "': "
			<< pam_strerror( appdata.h, rc );
		return NULL;
	}

	for( int i = 0; i < 10; i++ ) {
		rc = pam_authenticate( appdata.h, 0 );
#ifdef PAM_INCOMPLETE
		if( rc != PAM_INCOMPLETE )
#endif
			break;

		sleep( 1 );

		// we need a password
		//~ appdata.has_pass = true;
		//~ appdata.pass = password;
	}

	if( rc != PAM_SUCCESS ) {
		LOG_ERROR << "pam_authenticate failed with service '" << m_service << "': "
			<< pam_strerror( appdata.h, rc ) << ", "
			<< appdata.errmsg;
		return NULL;
	}

	// is access to the account permitted?
	rc = pam_acct_mgmt( appdata.h, 0 );
	if( rc != PAM_SUCCESS ) {
		LOG_ERROR << "pam_acct_mgmt failed with service '" << m_service << "': "
			<< pam_strerror( appdata.h, rc );
		return NULL;
	}

	// terminate PAM session with last exit code
	if( pam_end( appdata.h, rc ) != PAM_SUCCESS ) {
		LOG_ERROR << "pam_end failed with service '" << m_service << "': "
			<< pam_strerror( appdata.h, rc );
	}

	return new User( identifier(), "WOLFRAME-PAM", appdata.login, "" );
}

#define STRINGIFY( name ) # name
static const char *m_sliceStateToString[] = {
	STRINGIFY( PAMAuthSlice::SLICE_INITIALIZED ),
	STRINGIFY( PAMAuthSlice::SLICE_WAITING_FOR_PWD ),
	STRINGIFY( PAMAuthSlice::SLICE_USER_NOT_FOUND ),
	STRINGIFY( PAMAuthSlice::INVALID_CREDENTIALS ),
	STRINGIFY( PAMAuthSlice::SLICE_AUTHENTICATED ),
	STRINGIFY( PAMAuthSlice::SLICE_SYSTEM_FAILURE )
};

PAMAuthSlice::PAMAuthSlice( const PAMAuthUnit& backend )
	: m_backend( backend )
{
	m_appdata.h = NULL;
	m_appdata.has_pass = false;
	m_appdata.pass = "";
	m_conv.conv = pam_conv_func;
	m_conv.appdata_ptr = &m_appdata;
	m_inputReusable = false;

	m_state = SLICE_INITIALIZED;
}

PAMAuthSlice::~PAMAuthSlice()
{
}

void PAMAuthSlice::dispose()
{
	delete this;
}

/// The input message
void PAMAuthSlice::messageIn( const std::string& message )
{
	int rc = 0;

	switch( m_state ) {
		case SLICE_INITIALIZED:
			m_appdata.login = message.c_str( );
			// TODO: the service name must be a CONSTANT due to security reasons!
			rc = pam_start( m_backend.m_service.c_str( ), m_appdata.login.c_str( ), &m_conv, &m_appdata.h );
			if( rc != PAM_SUCCESS ) {
				LOG_ERROR << "PAM auth slice: "
					<< "pam_start failed with service " << m_backend.m_service << ": "
					<< pam_strerror( m_appdata.h, rc ) << " in state '"
					<< m_sliceStateToString[m_state] << "'";
				m_state = SLICE_SYSTEM_FAILURE;
			} else {
				m_state = SLICE_ASK_FOR_PASSWORD;
			}
			break;

		case SLICE_WAITING_FOR_PWD:
			m_appdata.has_pass = true;
			m_appdata.pass = message.c_str( );

			rc = pam_authenticate( m_appdata.h, 0 );
			if( rc == PAM_SUCCESS ) {
				m_state = SLICE_AUTHENTICATED;
			} else if( rc == PAM_USER_UNKNOWN ) {
				m_state = SLICE_USER_NOT_FOUND;
			} else if( rc == PAM_AUTH_ERR ) {
				m_state = SLICE_INVALID_CREDENTIALS;
			} else if( rc != PAM_SUCCESS ) {
				LOG_ERROR << "pam_authenticate failed with service '" << m_backend.m_service << "': "
					<< pam_strerror( m_appdata.h, rc ) << ", "
					<< m_appdata.errmsg;
				m_state = SLICE_SYSTEM_FAILURE;
			}

			if( rc == PAM_SUCCESS ) {
				rc = pam_acct_mgmt( m_appdata.h, 0 );
				if( rc != PAM_SUCCESS ) {
					LOG_ERROR << "pam_acct_mgmt failed with service '" << m_backend.m_service << "': "
						<< pam_strerror( m_appdata.h, rc ) << ", "
						<< m_appdata.errmsg;
					m_state = SLICE_SYSTEM_FAILURE;
				}
			}

			// get the name of the user as reported by PAM
			if( rc == PAM_SUCCESS ) {
				union { const char *s; const void *v; } user_union;
#ifdef SUNOS
				rc = pam_get_item( m_appdata.h, PAM_USER, (void **)&user_union.v );
#else
				rc = pam_get_item( m_appdata.h, PAM_USER, &user_union.v );
#endif
				if( rc != PAM_SUCCESS || user_union.v == NULL ) {
					LOG_WARNING << "PAM auth slice: can't get name of user (PAM_USER) using login instead";
					user_union.s = m_appdata.login.c_str( );
				}

				m_user = std::string( user_union.s );
			}

			// terminate PAM session with last exit code
			if( pam_end( m_appdata.h, rc ) != PAM_SUCCESS ) {
				LOG_ERROR << "pam_end failed with service '" << m_backend.m_service << "': "
					<< pam_strerror( m_appdata.h, rc ) << ", "
					<< m_appdata.errmsg;
				m_state = SLICE_SYSTEM_FAILURE;
			}
			m_appdata.h = NULL;

			break;

		case SLICE_ASK_FOR_PASSWORD:
		case SLICE_USER_NOT_FOUND:
		case SLICE_INVALID_CREDENTIALS:
		case SLICE_AUTHENTICATED:
		case SLICE_SYSTEM_FAILURE:
			LOG_ERROR << "PAM auth slice: receiving unexpected message in state '"
				<< m_sliceStateToString[m_state] << "', this is illegal!";
			break;
	}
}

/// The output message
std::string PAMAuthSlice::messageOut()
{
	switch( m_state ) {
		case SLICE_ASK_FOR_PASSWORD:
			m_state = SLICE_WAITING_FOR_PWD;
			return "password?";

		case SLICE_INITIALIZED:
		case SLICE_WAITING_FOR_PWD:
		case SLICE_USER_NOT_FOUND:
		case SLICE_INVALID_CREDENTIALS:
		case SLICE_AUTHENTICATED:
		case SLICE_SYSTEM_FAILURE:
			LOG_ERROR << "PAM auth slice: receiving unexpected message in state '"
				<< m_sliceStateToString[m_state] << "', this is illegal!";
			break;

		default:
			LOG_FATAL << "PAM auth slice: receiving unexpected message in illegal state "
				<< m_state << "!";
	}

	return std::string( );
}

/// The current status of the authenticator slice
AuthenticatorSlice::Status PAMAuthSlice::status() const
{
	switch( m_state ) {
		case SLICE_INITIALIZED:
			return AWAITING_MESSAGE;

		case SLICE_ASK_FOR_PASSWORD:
			return MESSAGE_AVAILABLE;

		case SLICE_WAITING_FOR_PWD:
			return AWAITING_MESSAGE;

		case SLICE_USER_NOT_FOUND:
			return USER_NOT_FOUND;

		case SLICE_INVALID_CREDENTIALS:
			return INVALID_CREDENTIALS;

		case SLICE_AUTHENTICATED:
			return AUTHENTICATED;

		case SLICE_SYSTEM_FAILURE:
			return SYSTEM_FAILURE;

		default:
			LOG_FATAL << "PAM auth slice: called status( ) in illegal state "
				<< m_state << "!";
			return SYSTEM_FAILURE;
	}
	return SYSTEM_FAILURE;
}

/// The authenticated user or NULL if not authenticated
User* PAMAuthSlice::user()
{
	if ( m_state == SLICE_AUTHENTICATED )	{
		if( m_user.empty( ) ) {
			return NULL;
		}

		User *usr = new User( identifier(), "WOLFRAME-PAM", m_user, "" );

		m_user.clear( );

		return usr;
	} else {
		return NULL;
	}
}

}} // namespace _Wolframe::AAAA
