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
/// \file usernameHash.cpp
/// Username Hash Implementation

#include <stdexcept>
#include <cstring>

#include "AAAA/usernameHash.hpp"
#include "types/byte2hex.h"
#include "types/base64.hpp"
#include "crypto/HMAC.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe::AAAA;
using namespace _Wolframe;

static const size_t USERNAME_HASH_BCD_SIZE = 2 * USERNAME_HASH_SIZE ;
static const size_t USERNAME_HASH_BASE64_SIZE = (( USERNAME_HASH_SIZE - 1 ) / 3 ) * 4 + 5;

static const size_t USERNAME_SALT_BCD_SIZE = 2 * USERNAME_HASH_SALT_SIZE;
static const size_t USERNAME_SALT_BASE64_SIZE = (( USERNAME_HASH_SALT_SIZE - 1 ) / 3 ) * 4 + 5;


UsernameHash::UsernameHash( const crypto::RandomGenerator& rndGen,
			    const std::string& username )
{
	rndGen.generate( m_salt, sizeof( m_salt ) );
	m_saltSize = sizeof( m_salt );
	hmac_sha256( m_salt, m_saltSize, (const unsigned char *)username.data(),
		     username.length(), m_hash );
}

UsernameHash::UsernameHash( const unsigned char *slt, std::size_t sltSize,
			    const std::string& username )
{
	memset( m_salt, 0, sizeof( m_salt ) );
	m_saltSize = sltSize > sizeof( m_salt ) ? sizeof( m_salt ) : sltSize;
	memcpy( m_salt, slt, m_saltSize );
	hmac_sha256( m_salt, m_saltSize, (const unsigned char *)username.data(),
		     username.length(), m_hash );
}

UsernameHash::UsernameHash( const std::string& hashString )
{
	memset( m_salt, 0, sizeof( m_salt ));
	memset( m_hash, 0, sizeof( m_hash ));

	int sltSize;
	int hshSize;

	std::string s = boost::algorithm::trim_copy( hashString );
	if ( s[ 0 ] == '$' )	{
		size_t hashStart = s.find( "$", 1 );
		if ( hashStart == s.npos )	{
			std::string errMsg = "'" + s + "' is not a valid username hash string";
			throw std::runtime_error( errMsg );
		}
		if (( sltSize = base64::decode( s.substr( 1, hashStart - 1 ),
						 m_salt, sizeof( m_salt ) )) < 0 )	{
			std::string errMsg = "'" + s + "' is not a valid username hash string (salt error)";
			throw std::runtime_error( errMsg );
		}
		m_saltSize = ( std::size_t )sltSize;
		if (( hshSize = base64::decode( s.substr( hashStart + 1 ),
						 m_hash, sizeof( m_hash ) )) < 0 )	{
			std::string errMsg = "'" + s + "' is not a valid username hash string (hash error)";
			throw std::runtime_error( errMsg );
		}
		if ( (size_t)hshSize != sizeof( m_hash ) )	{
			std::string errMsg = "'" + s + "' is not a valid challenge string (expected "
					     + boost::lexical_cast<std::string>( sizeof( m_hash ) )
					     + " bytes, got "
					     + boost::lexical_cast<std::string>( hshSize ) + ")";
			throw std::runtime_error( errMsg );
		}
	}
	else	{
		std::string errMsg = "'" + s + "' is not a valid username hash string";
		throw std::runtime_error( errMsg );
	}
}

UsernameHash::~UsernameHash()
{
	memset( m_salt, 0, sizeof( m_salt ) );
	memset( m_hash, 0, sizeof( m_hash ) );
}

std::string UsernameHash::toBCD() const
{
	static const std::size_t bufSize = USERNAME_HASH_BCD_SIZE + 1 + USERNAME_SALT_BCD_SIZE + 1;
	char	buffer[ bufSize ];

	int len = byte2hex( m_salt, sizeof( m_salt ),
			    buffer, USERNAME_SALT_BCD_SIZE );
	assert( len == USERNAME_HASH_SALT_SIZE * 2 );

	buffer[ USERNAME_SALT_BCD_SIZE ] = ':';

	len = byte2hex( m_hash, sizeof( m_hash ), buffer + USERNAME_SALT_BCD_SIZE + 1,
			USERNAME_HASH_BCD_SIZE );
	assert( len == USERNAME_HASH_SIZE * 2 );

	return std::string( buffer );
}


std::string UsernameHash::toString() const
{
	char	bufSalt[ USERNAME_SALT_BASE64_SIZE ];
	int len = base64_encode( m_salt, sizeof( m_salt ),
				 bufSalt, USERNAME_SALT_BASE64_SIZE, 0 );
	assert( len >= 0 && len < (int)USERNAME_SALT_BASE64_SIZE );
	while ( len > 0 && bufSalt[ len - 1 ] == '=' )
		len--;
	bufSalt[ len ] = 0;

	char	bufHash[ USERNAME_HASH_BASE64_SIZE ];
	len = base64_encode( m_hash, sizeof( m_hash ),
			     bufHash, USERNAME_HASH_BASE64_SIZE, 0 );
	assert( len >= 0 && len < (int)USERNAME_HASH_BASE64_SIZE );
	while ( len > 0 && bufHash[ len - 1 ] == '=' )
		len--;
	bufHash[ len ] = 0;

	return std::string( "$" ) + std::string( bufSalt ) + "$" + std::string( bufHash );
}

