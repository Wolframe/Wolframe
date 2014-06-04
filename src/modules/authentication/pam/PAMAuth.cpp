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

#include <string>
#include <cstring>
#include <ostream>

#include "logger-v1.hpp"
#include "PAMAuth.hpp"

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
				if( !appdata->has_pass )
// Solaris and NetBSD have no PAM_CONV_AGAIN, returning an error instead
#if defined SUNOS || NETBSD
					return PAM_CONV_ERR;
#else
					return PAM_CONV_AGAIN;
#endif

				r->resp = strdup( appdata->pass.c_str( ) );
				if( r->resp == NULL ) {
					appdata->errmsg = "Unable to allocate memory for password answer";
					goto error;
				}
				break;

// Check against the PAM_USER_PROMPT to be sure we have a login request.
// Always recheck because the library could change the prompt any time
			case PAM_PROMPT_ECHO_ON:
// Solaris hat different API in pam_get_item
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
				if( strcmp( m->msg, login_prompt ) == 0 ) {
					r->resp = strdup( appdata->login.c_str( ) );
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

//*********   PAM Authentication Unit   *************************************

PAMAuthUnit::PAMAuthUnit( const std::string& Identifier,
				    const std::string& service )
	: AuthenticationUnit( Identifier ), m_service( service )
{
	m_appdata.h = NULL;
	m_appdata.has_pass = false;
	m_appdata.pass = "";
	m_conv.conv = pam_conv_func;
	m_conv.appdata_ptr = &m_appdata;

	m_state = _Wolframe_PAM_STATE_NEED_LOGIN;

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

}} // namespace _Wolframe::AAAA
