#ifndef WTPACMAN_AUTH_PAM_H
#define WTPACMAN_AUTH_PAM_H

#ifdef __cplusplus
extern "C" {
#endif

#define _XOPEN_SOURCE 500
#include <string.h>

#include <security/pam_appl.h> 

/**
 * Login via PAM.
 */
int auth_pam(		const char *login,
				const char *password,
				const char *service,
				char *errbuf, size_t errbuflen );

#ifdef __cplusplus
}
#endif

#endif /* ifndef WTPACMAN_AUTH_PAM_H */
