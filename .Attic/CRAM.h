/*
 * Challenge Response Authentication Mechanism
 *
 * This is using a simple HMAC-SHA256
 */

#ifndef _CRAM_H_INCLUDED
#define _CRAM_H_INCLUDED


#define	HASH_BLOCK_SIZE			64
#define	HASH_DIGEST_SIZE		32
#define	PASSWORD_HASH_SIZE		HASH_DIGEST_SIZE
#define	CRAM_CHALLENGE_SIZE		HASH_BLOCK_SIZE
#define	CRAM_RESPONSE_SIZE		HASH_DIGEST_SIZE

#define	PASSWORD_HASH_STRING_SIZE	2 * PASSWORD_HASH_SIZE + 1
#define	CRAM_CHALLENGE_STRING_SIZE	2 * CRAM_CHALLENGE_SIZE + 1
#define	CRAM_RESPONSE_STRING_SIZE	2 * CRAM_RESPONSE_SIZE + 1


#include <stdlib.h>


#ifdef	__cplusplus
extern "C" {
#endif


char *passwordHash( char *password, char *buffer, size_t size );

char *CRAMchallenge( char *buffer, size_t size );

char *CRAMresponse( const char *challenge, const char *passwordHash, char *buffer, size_t size );


#ifdef __cplusplus
}
#endif

#endif	/* _CRAM_H_INCLUDED */

