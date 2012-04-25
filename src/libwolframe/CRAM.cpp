/******************************************************************************

	Challenge Response Authentication Mechanism Implementation

*******************************************************************************/
/**
 * @defgroup CRAM Challenge Response Authentication Mechanism functions
 * @ingroup miscFunctions
 * @{
 */

/**
 * @file CRAM.c
 * @brief Implementation of the Challenge Response Authentication Mechanism
 * @author Mihai Barbos <mihai.barbos@gmail.com>
 */

#include "CRAM.h"
#include "byte2hex.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/sha.h>


/**
 * @brief	Get a challenge message for CRAM
 *
 * @note	This function reads from /dev/urandom.
 *		I haven't got the slightest idea what to do if that fails
 *
 * @param	buffer		pointer to a buffer which will hold the challenge
 * @param	size		size of the buffer
 *
 * @return	pointer to the output buffer (string) if successful
 *		or NULL if the output buffer is not large enough
 */
char *CRAMchallenge( char *buffer, size_t size )
{
	int		fd;
	unsigned char	buf[CRAM_CHALLENGE_SIZE];

	if ( ! size >= CRAM_CHALLENGE_STRING_SIZE )
		return( NULL );

	fd = open( "/dev/urandom", O_RDONLY );
	read( fd, buf, CRAM_CHALLENGE_SIZE );
	close( fd );

	return( byte2hex( buf, CRAM_CHALLENGE_SIZE, buffer, size ));
}


/**
 * @brief	Compute the hash of a password (ASCII hex)
 *
 * @param	password	password in clear
 * @param	buffer		pointer to a buffer which will hold the hash
 * @param	size		size of the output buffer
 *
 * @return	pointer to the output string if successful
 *		or NULL if the output buffer is not large enough
 *
 * @note	The password string will be wiped out after this function call
 *		no matter what the result will be
 */
char *passwordHash( char *password, char *buffer, size_t size )
{
	unsigned char	digest[PASSWORD_HASH_SIZE];
	size_t		len;

	if (size < 2 * PASSWORD_HASH_SIZE + 1 )	{
		while( *password )
			*password++ = 0;
		return( NULL );
	}

	if (( len = strlen( password )) == 0 )
		return( NULL );

	if ( len >= HASH_BLOCK_SIZE )
		SHA256(( unsigned char *)password, len, digest );
	else	{
		unsigned char	mangle[HASH_BLOCK_SIZE + 1];
		char		*p = password;
		len = 0;
		while ( len < HASH_BLOCK_SIZE )	{
			mangle[len] = (unsigned char)*p;
			len++, p++;
			if ( *p == 0 )
				p = password;
		}
		SHA256( mangle, HASH_BLOCK_SIZE, digest );
	}
	while( *password )
		*password++ = 0;

	return( byte2hex( digest, PASSWORD_HASH_SIZE, buffer, size ));
}


/**
 * @brief	Compute the challenge response for CRAM
 *
 * @param	challenge	the CRAM challenge (ASCII hex)
 * @param	pwdHash		the hashed password (ASCII hex)
 * @param	buffer		pointer to the buffer for the output string
 * @param	size		size of the output buffer
 *
 * @return	pointer to the output string if successful
 *		or NULL if the output buffer is not large enough
 */
char *CRAMresponse( const char *challenge, const char *pwdHash, char *buffer, size_t size )
{
	int		i;
	unsigned char	datablk[CRAM_CHALLENGE_SIZE];
	unsigned char	pwdblk[CRAM_CHALLENGE_SIZE];
	unsigned char	response[CRAM_RESPONSE_SIZE];

	if (size < 2 * CRAM_RESPONSE_SIZE + 1)
		return( NULL );

	hex2byte( challenge, datablk, CRAM_CHALLENGE_SIZE );
	hex2byte( pwdHash, pwdblk, PASSWORD_HASH_SIZE );
	memcpy( pwdblk, pwdblk + PASSWORD_HASH_SIZE, PASSWORD_HASH_SIZE );
	for ( i = 0; i < CRAM_CHALLENGE_SIZE; i++ )
		datablk[i] ^= pwdblk[i];
	SHA256( datablk, CRAM_CHALLENGE_SIZE, response );
	return( byte2hex( response, CRAM_RESPONSE_SIZE, buffer, size ));
}


/** @} *//* CRAM */

