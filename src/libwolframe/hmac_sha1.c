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
///
/// Standard HMAC-SHA1 implementation
///

#include <string.h>

#include "crypto/HMAC.h"
#include "crypto/sha1.h"

#define HMAC_SHA1_BLOCK_SIZE	SHA1_BLOCK_SIZE

void hmac_sha1( const unsigned char *key, size_t keyLen,
		const unsigned char *msg, size_t msgLen,
		unsigned char hash[] )
{
	unsigned char	pad[ HMAC_SHA1_BLOCK_SIZE ];
	unsigned char	normalizedKey[ HMAC_SHA1_BLOCK_SIZE ];
	unsigned char	intermediateHash[ HMAC_SHA1_HASH_SIZE ];
	size_t		i;
	sha1_ctx	ctx;

	memset( normalizedKey, 0, HMAC_SHA1_BLOCK_SIZE );
	if ( keyLen > HMAC_SHA1_BLOCK_SIZE )
		sha1( key, keyLen, normalizedKey );
	else	{
		for ( i = 0; i < keyLen; i ++ )
			normalizedKey[ i ] = key[ i ];
	}

	memset( pad, 0x36, HMAC_SHA1_BLOCK_SIZE );
	for ( i = 0; i < HMAC_SHA1_BLOCK_SIZE; i ++ )
		pad[ i ] ^= normalizedKey[ i ];
	sha1_init( &ctx );
	sha1_update( &ctx, pad, HMAC_SHA1_BLOCK_SIZE );
	sha1_update( &ctx, msg, msgLen );
	sha1_final( &ctx, intermediateHash );

	memset( pad, 0x5c, HMAC_SHA1_BLOCK_SIZE );
	for ( i = 0; i < HMAC_SHA1_BLOCK_SIZE; i ++ )
		pad[ i ] ^= normalizedKey[ i ];
	sha1_init( &ctx );
	sha1_update( &ctx, pad, HMAC_SHA1_BLOCK_SIZE );
	sha1_update( &ctx, intermediateHash, HMAC_SHA1_HASH_SIZE );
	sha1_final( &ctx, hash );

	memset( pad, 0, HMAC_SHA1_BLOCK_SIZE );
	memset( normalizedKey, 0, HMAC_SHA1_BLOCK_SIZE );
	memset( intermediateHash, 0, HMAC_SHA1_BLOCK_SIZE );
}
