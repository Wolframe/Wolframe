#include "authentication_pam.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	const char *login;
	const char *password;
	char *errbuf;
	size_t errbuflen;
	pam_handle_t *h;
} pam_appdata;

static const char *msg_style_to_str( int msg_style ) {
	switch( msg_style ) {
		case PAM_PROMPT_ECHO_OFF:	return "PAM_PROMPT_ECHO_OFF";
		case PAM_TEXT_INFO:		return "PAM_TEXT_INFO";
		case PAM_ERROR_MSG:		return "PAM_ERROR_MSG";
		case PAM_PROMPT_ECHO_ON:	return "PAM_PROMPT_ECHO_ON";
		default:			return "<unknown msg type>";
	}
}

static void null_and_free( int nmsg, struct pam_response *pr ) {
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

static int pam_conv_func( int nmsg, const struct pam_message **msg,
	struct pam_response **reply, void *appdata_ptr ) {
	int i;
	const struct pam_message *m = *msg;
	struct pam_response *r;
	int rc;
	const char *login_prompt = NULL;

	pam_appdata *appdata = (pam_appdata *)appdata_ptr;

	/* check sanity of the messages passed */
	if( nmsg <= 0 || nmsg >= PAM_MAX_NUM_MSG ) {
		snprintf( appdata->errbuf, appdata->errbuflen, "bad number of messages in PAM conversion function: %d (0>=%d<=%d)\n",
			nmsg, nmsg, PAM_MAX_NUM_MSG );
		*reply = NULL;
		return PAM_CONV_ERR;
	}

	/* allocate return messages */
	if( ( *reply = r = (struct pam_response *)calloc( nmsg, sizeof( struct pam_response ) ) ) == NULL ) {
		snprintf( appdata->errbuf, appdata->errbuflen, "Unable to allocate memory for replies" );
		return PAM_BUF_ERR;
	}

	for( i = 0; i < nmsg; i++ ) {
		if( msg[i]->msg == NULL ) {
			snprintf( appdata->errbuf, appdata->errbuflen, "bad message %d of type %s, is not supposed to be NULL!\n", i,
				msg_style_to_str( m->msg_style ) );
			goto error;
		}

		/* initalize response */
		r->resp = NULL;
		r->resp_retcode = 0;

		switch( msg[i]->msg_style ) {
			/* Usually we get prompted for a password, this is not always true though. */
			case PAM_PROMPT_ECHO_OFF:
				r->resp = strdup( appdata->password );
				if( r->resp == NULL ) {
					snprintf( appdata->errbuf, appdata->errbuflen, "Unable to allocate memory for password answer" );
					goto error;
				}
				break;

			/* Check against the PAM_USER_PROMPT to be sure we have a login request.
			 * Always recheck because the library could change the prompt any time
			 */
			case PAM_PROMPT_ECHO_ON:
				rc = pam_get_item( appdata->h, PAM_USER_PROMPT, &login_prompt );
				if( rc != PAM_SUCCESS ) {
					snprintf( appdata->errbuf, appdata->errbuflen,
						"pam_get_item( PAM_USER_PROMPT) failed with: %s (%d)\n",
						pam_strerror( appdata->h, rc ), rc );
					goto error;
				}
				if( strcmp( m->msg, login_prompt ) == 0 ) {
					r->resp = strdup( appdata->login );
					if( r->resp == NULL ) {
						snprintf( appdata->errbuf, appdata->errbuflen, "Unable to allocate memory for login answer" );
						goto error;
					}
				}
				break;

			case PAM_ERROR_MSG:
			case PAM_TEXT_INFO:
				/* ignore, maybe log later? */
				break;

			default:
				snprintf( appdata->errbuf, appdata->errbuflen, "unknown message %d of type %s: %s\n",
					i, msg_style_to_str( m->msg_style ), m->msg );
				goto error;

			/* next message and response */
			m++; r++;
		}
	}

	return PAM_SUCCESS;

error:
	null_and_free( i, r );
	*reply = NULL;
	return PAM_CONV_ERR;
}

int auth_pam(		const char *login,
			const char *password,
			const char *service,
			char *errbuf, size_t errbuflen ) {
	int rc;
	pam_appdata appdata;
	struct pam_conv conv;
	pam_handle_t *h;

	appdata.login = login;
	appdata.password = password;
	appdata.errbuf = errbuf;
	appdata.errbuflen = errbuflen;
	appdata.h = NULL;

	conv.conv = pam_conv_func;
	conv.appdata_ptr = &appdata;

	/* the service name must be a CONSTANT due to security reasons! */
	rc = pam_start( service, login, &conv, &h );
	if( rc != PAM_SUCCESS ) {
		snprintf( errbuf, errbuflen, "pam_start failed with: %s (%d)\n", pam_strerror( h, rc ), rc );
		return rc;
	}
	appdata.h = h;

	/* authenticate: are we who we claim to be? */
	rc = pam_authenticate( h, 0 );
	if( rc != PAM_SUCCESS ) {
		snprintf( errbuf, errbuflen, "pam_authenticate failed with: %s (%d)\n", pam_strerror( h, rc ), rc );
		goto TERMINATE;
	}

	/* is access permitted? */
	rc = pam_acct_mgmt( h, 0 );
	if( rc != PAM_SUCCESS ) {
		fprintf( stderr, "pam_acct_mgmt failed with: %s (%d)\n", pam_strerror( h, rc ), rc );
		goto TERMINATE;
	}

TERMINATE:
	/* terminate PAM session with last exit code */
	if( pam_end( h, rc ) != PAM_SUCCESS ) {
		snprintf( errbuf, errbuflen, "pam_end failed with: %s (%d)\n", pam_strerror( h, rc ), rc );
	}

	return( rc == PAM_SUCCESS ? 0 : 1 );
}
