#ifndef SMERP__AUTH_PAM_H
#define SMERP__AUTH_PAM_H

#ifdef __cplusplus
extern "C" {
#endif

#define _XOPEN_SOURCE 500
#include <string.h>

#include <security/pam_appl.h> 

/**
 * Login via PAM.
 *
 */
int smerp_auth_pam(	const char *login,
			const char *password,
			const char *service,
			char *errbuf,
			size_t errbuflen );

#ifdef __cplusplus
}
#endif

#endif /* ifndef SMERP__AUTH_PAM_H */
