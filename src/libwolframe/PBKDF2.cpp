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
/// PBKDF2-HMAC-SHA1 C++ interface
///

#include <cstring>
#include <cassert>
#include <stdexcept>

#include "crypto/PBKDF2.hpp"
#include "crypto/PBKDF2.h"

#include "types/byte2hex.h"
#include "types/base64.hpp"

namespace _Wolframe {
namespace crypto {

PBKDF2_HMAC_SHA1::PBKDF2_HMAC_SHA1( const unsigned char* salt, size_t saltSize,
				    const unsigned char* password, size_t pwdSize,
				    std::size_t dkLen, unsigned int rounds )
{
	m_hash = new unsigned char[ dkLen ];
	if ( pbkdf2_hmac_sha1( salt, saltSize, password, pwdSize, dkLen, rounds,
			       m_hash ) != 0 )	{
		delete[] m_hash; m_hash = NULL; m_dkLen = 0;
		throw std::runtime_error( "error building PBKDF2-HMAC-SHA1 derived key" );
	}
	m_dkLen = dkLen;
}

PBKDF2_HMAC_SHA1::PBKDF2_HMAC_SHA1( const std::string& salt, const std::string& password,
				    std::size_t dkLen, unsigned int rounds )
{
	m_hash = new unsigned char[ dkLen ];
	if ( pbkdf2_hmac_sha1( (const unsigned char*)salt.data(), salt.size(),
			       (const unsigned char*)password.data(), password.size(),
			       dkLen, rounds, m_hash ) != 0 )	{
		delete[] m_hash; m_hash = NULL; m_dkLen = 0;
		throw std::runtime_error( "error building PBKDF2-HMAC-SHA1 derived key" );
	}
	m_dkLen = dkLen;
}

PBKDF2_HMAC_SHA1::PBKDF2_HMAC_SHA1( const unsigned char* salt, size_t saltSize,
				    const std::string& password,
				    std::size_t dkLen, unsigned int rounds )
{
	m_hash = new unsigned char[ dkLen ];
	if ( pbkdf2_hmac_sha1( salt, saltSize,
			       (const unsigned char*)password.data(), password.size(),
			       dkLen, rounds, m_hash ) != 0 )	{
		delete[] m_hash; m_hash = NULL; m_dkLen = 0;
		throw std::runtime_error( "error building PBKDF2-HMAC-SHA1 derived key" );
	}

	m_dkLen = dkLen;
}

PBKDF2_HMAC_SHA1::PBKDF2_HMAC_SHA1( const std::string& salt,
				    const unsigned char* password, size_t pwdSize,
				    std::size_t dkLen, unsigned int rounds )
{
	m_hash = new unsigned char[ dkLen ];
	if ( pbkdf2_hmac_sha1( (const unsigned char*)salt.data(), salt.size(),
			       password, pwdSize, dkLen, rounds, m_hash ) != 0 )	{
		delete[] m_hash; m_hash = NULL; m_dkLen = 0;
		throw std::runtime_error( "error building PBKDF2-HMAC-SHA1 derived key" );
	}
	m_dkLen = dkLen;
}


PBKDF2_HMAC_SHA1::PBKDF2_HMAC_SHA1( const std::string& str )
{
	size_t maxSize = (( str.length() + 3 ) * 3 ) / 4;
	m_hash = new unsigned char[ maxSize ];
	memset( m_hash, 0, maxSize );

	int ret;
	if (( ret = base64_decode( str.data(), str.size(),
				   m_hash, maxSize )) < 0 )	{
		delete[] m_hash; m_hash = NULL; m_dkLen = 0;
		std::string errMsg = "cannot convert '" + str + "' to a PBKDF2-HMAC-SHA1";
		throw std::runtime_error( errMsg );
	}
	m_dkLen = ret;
}


PBKDF2_HMAC_SHA1::~PBKDF2_HMAC_SHA1()
{
	if ( m_hash )	{
		delete[] m_hash;
		m_hash = NULL;
		m_dkLen = 0;
	}
}

std::string PBKDF2_HMAC_SHA1::toBCD() const
{
	char* buffer = new char[ m_dkLen * 2 + 1 ];

	int len = byte2hex( m_hash, m_dkLen, buffer, m_dkLen * 2 + 1 );
	assert( len == (int)m_dkLen * 2 );

	std::string ret( buffer );
	delete[] buffer;

	return ret;
}

std::string PBKDF2_HMAC_SHA1::toString() const
{
	std::size_t maxSize = (( m_dkLen - 1 ) / 3 ) * 4 + 5;
	char* buffer = new char[ maxSize ];

	int len = base64::encode( m_hash, m_dkLen, buffer, maxSize, 0 );
	assert( len > 0 && len < (int)maxSize );

	while ( len > 0 && buffer[ len - 1 ] == '=' )
		len--;
	buffer[ len ] = 0;

	std::string ret( buffer );
	delete[] buffer;

	return ret;
}

bool PBKDF2_HMAC_SHA1::operator == ( const PBKDF2_HMAC_SHA1& rhs ) const
{
	if ( m_dkLen != rhs.m_dkLen )
		return false;
	return !memcmp( this->m_hash, rhs.m_hash, m_dkLen );
}

bool PBKDF2_HMAC_SHA1::operator == ( const std::string& rhs ) const
{
	char* buffer = new char[ m_dkLen ];
	memset( buffer, 0, m_dkLen );

	int ret;
	if (( ret = base64_decode( rhs.data(), rhs.size(), buffer, m_dkLen )) < 0 )	{
		delete[] buffer;
		return false;
	}
	if ( ret != (int)m_dkLen )	{
		delete[] buffer;
		return false;
	}
	bool isEqual = !memcmp( this->m_hash, buffer, m_dkLen );
	delete[] buffer;
	return isEqual;
}

}} // namespace _Wolframe::crypto
