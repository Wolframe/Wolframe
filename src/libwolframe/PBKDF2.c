/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
/*
 *  Password-Based Key Derivation Function 2 (PKCS #5 v2.0).
 *  See RFC 2898 (http://tools.ietf.org/html/rfc2898)
 */

#include "types/PBKDF2.h"
#include "types/HMAC.h"

#include <string.h>
#include <limits.h>
#include <stdlib.h>

int pbkdf2( const unsigned char *salt, size_t saltLen,
	    const unsigned char *password, size_t pwdLen,
	    unsigned char *derivedKey, size_t dkLen, unsigned int rounds )
{
	unsigned char *asalt;
	unsigned char obuf[ HMAC_SHA256_DIGEST_SIZE ];
	unsigned char d1[ HMAC_SHA256_DIGEST_SIZE ], d2[ HMAC_SHA256_DIGEST_SIZE ];
	unsigned int i, j;
	unsigned int count;
	size_t r;

	if ( rounds < 1 || dkLen == 0 )
		return -1;
	if ( saltLen == 0 || saltLen > UINT_MAX - 4 )
		return -1;
	if (( asalt = (unsigned char *)malloc( saltLen + 4 )) == NULL )
		return -1;

	memcpy( asalt, salt, saltLen );

	for ( count = 1; dkLen > 0; count++ )	{
		asalt[ saltLen + 0 ] = ( count >> 24 ) & 0xff;
		asalt[ saltLen + 1 ] = ( count >> 16 ) & 0xff;
		asalt[ saltLen + 2 ] = ( count >> 8 ) & 0xff;
		asalt[ saltLen + 3 ] = count & 0xff;
		hmac_sha256( asalt, saltLen + 4, password, pwdLen, d1 );
		memcpy( obuf, d1, sizeof( obuf ));

		for ( i = 1; i < rounds; i++ ) {
			hmac_sha256( d1, sizeof( d1 ), password, pwdLen, d2 );
			memcpy( d1, d2, sizeof( d1 ));
			for ( j = 0; j < sizeof( obuf ); j++ )
				obuf[ j ] ^= d1[ j ];
		}

		r = dkLen < HMAC_SHA256_DIGEST_SIZE ? dkLen : HMAC_SHA256_DIGEST_SIZE;
		memcpy( derivedKey, obuf, r );
		derivedKey += r;
		dkLen -= r;
	};
	memset( asalt, 0, saltLen + 4 );
	free( asalt );
	memset( d1, 0, sizeof( d1 ));
	memset( d2, 0, sizeof( d2 ));
	memset( obuf, 0, sizeof( obuf ));

	return 0;
}
