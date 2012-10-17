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
///
///
///

#include <cstring>
#include <cassert>
#include <stdexcept>

#include "AAAA/HMAC.hpp"

#include "types/byte2hex.h"
#include "types/base64.hpp"
#include "types/sha2.h"

namespace _Wolframe {
namespace AAAA {

void HMAC_SHA256::init( const unsigned char *key, size_t keyLen,
		   const unsigned char *msg, size_t msgLen )
{
	unsigned char	pad[ HMAC_BLOCK_SIZE ];
	unsigned char	intKey[ HMAC_BLOCK_SIZE ];
	unsigned char	hash[ HMAC_DIGEST_SIZE ];

	sha256_ctx	ctx;

	memset( intKey, 0, HMAC_BLOCK_SIZE );
	if ( keyLen > HMAC_BLOCK_SIZE )	{
		sha256( key, keyLen, intKey );
// This is non-standard
//		sha512( key, keyLen, intKey );
	}
	else	{
		for ( size_t i = 0; i < keyLen; i ++ )
			intKey[ i ] = key[ i ];
	}

	memset( pad, 0x36, HMAC_BLOCK_SIZE );
	for ( size_t i = 0; i < HMAC_BLOCK_SIZE; i ++ )
		pad[ i ] ^= intKey[ i ];
	sha256_init( &ctx );
	sha256_update( &ctx, pad, HMAC_BLOCK_SIZE );
	sha256_update( &ctx, msg, msgLen );
	sha256_final( &ctx, hash );

	memset( pad, 0x5c, HMAC_BLOCK_SIZE );
	for ( size_t i = 0; i < HMAC_BLOCK_SIZE; i ++ )
		pad[ i ] ^= intKey[ i ];
	sha256_init( &ctx );
	sha256_update( &ctx, pad, HMAC_BLOCK_SIZE );
	sha256_update( &ctx, hash, HMAC_DIGEST_SIZE );
	sha256_final( &ctx, m_HMAC );
}

std::string HMAC_SHA256::toBCD() const
{
	char	buffer[ HMAC_BCD_SIZE ];

	memset( buffer, 0, HMAC_BCD_SIZE );

	if ( byte2hex( m_HMAC, HMAC_DIGEST_SIZE,
		       buffer, HMAC_BCD_SIZE ) == NULL )
		throw std::logic_error( "HMAC-SHA256::toBCD() cannot convert hash ?!?" );

	return std::string( buffer );
}

std::string HMAC_SHA256::toBase64() const
{
	char	buffer[ HMAC_BASE64_SIZE ];

	memset( buffer, 0, HMAC_BASE64_SIZE );

	if ( base64::encode( m_HMAC, HMAC_DIGEST_SIZE,
			     buffer, HMAC_BASE64_SIZE, 0 ) < 0 )
		throw std::logic_error( "HMAC-SHA256::toBase64() cannot convert hash ?!?" );

	return std::string( buffer );
}

bool HMAC_SHA256::operator == ( const HMAC_SHA256& rhs ) const
{
	return !memcmp( this->m_HMAC, rhs.m_HMAC, HMAC_DIGEST_SIZE );
}

bool HMAC_SHA256::operator == ( const std::string& rhs ) const
{
	unsigned char	buffer[ HMAC_DIGEST_SIZE ];

	if ( hex2byte( rhs.data(), buffer, HMAC_DIGEST_SIZE ) != HMAC_DIGEST_SIZE )
		return false;
	return !memcmp( this->m_HMAC, buffer, HMAC_DIGEST_SIZE );
}

}} // namespace _Wolframe::AAAA
