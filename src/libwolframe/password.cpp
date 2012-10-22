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
//
/// \file password.cpp
/// Wolframe password hashes
//

#include <stdexcept>
#include <cstring>

#ifndef _WIN32
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#else
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <time.h>
#endif

#include "AAAA/password.hpp"
#include "types/byte2hex.h"
#include "types/base64.hpp"
#include "types/sha2.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace _Wolframe::AAAA;
using namespace _Wolframe;

static const size_t PASSWORD_SALT_BCD_SIZE = PASSWORD_SALT_SIZE * 2 + 1;
static const size_t PASSWORD_SALT_BASE64_SIZE = (( PASSWORD_SALT_SIZE * 4 ) / 3 ) +
			(( PASSWORD_SALT_SIZE % 3 ) ? ( 3 - ( PASSWORD_SALT_SIZE % 3 )) : 0 ) + 1;

static const size_t PASSWORD_HASH_BCD_SIZE = PASSWORD_HASH_SIZE * 2 + 1;
static const size_t PASSWORD_HASH_BASE64_SIZE = (( PASSWORD_HASH_SIZE * 4 ) / 3 ) +
			(( PASSWORD_HASH_SIZE % 3 ) ? ( 3 - ( PASSWORD_HASH_SIZE % 3 )) : 0 ) + 1;

PasswordSalt::PasswordSalt()
{
	m_size = 0;
	memset( m_salt, 0, PASSWORD_SALT_SIZE );
}

/// \note The byte array is considered to be of size PASSWORD_SALT_SIZE
/// and it can not be changed.
PasswordSalt::PasswordSalt( const unsigned char* salt , size_t bytes )
{
	m_size = bytes > PASSWORD_SALT_SIZE ? PASSWORD_SALT_SIZE : bytes;
	memcpy( m_salt, salt, m_size );
}

PasswordSalt::PasswordSalt( const std::string& randomDevice )
{
#ifndef _WIN32
	int hndl = open( randomDevice.c_str(), O_RDONLY );
	if ( hndl < 0 )	{
		std::string errMsg = "Error opening '" + randomDevice + "': ";
		throw std::runtime_error( errMsg );
	}

	int rndPart = read( hndl, m_salt, PASSWORD_SALT_SIZE );
	if ( rndPart < 0 )	{
		std::string errMsg = "Error reading '" + randomDevice + "'";
		throw std::runtime_error( errMsg );
	}
	else if ( rndPart < (int)PASSWORD_SALT_SIZE )	{
		std::string errMsg = "Not enough entropy in '" + randomDevice + "' ?!?";
		throw std::logic_error( errMsg );
	}

	close( hndl );
#else
	HCRYPTPROV provider = 0;

	if( !CryptAcquireContext( &provider, 0, randomDevice.c_str( ), PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT ) ) {
		throw std::runtime_error( "Error opening cyrpto context" );
	}

	if( !CryptGenRandom( provider, PASSWORD_SALT_SIZE, static_cast<BYTE *>( m_salt ))) {
		CryptReleaseContext( provider, 0 );
		throw std::runtime_error( "Error generating random data " );
	}

	if( !CryptReleaseContext( provider, 0 ) ) {
		throw std::runtime_error( "Error closing cyrpto context" );
	}
#endif
	m_size = PASSWORD_SALT_SIZE;
}

std::string PasswordSalt::toBCD() const
{
	char	buffer[ PASSWORD_SALT_BCD_SIZE ];

	if ( byte2hex( m_salt, PASSWORD_SALT_SIZE,
		       buffer, PASSWORD_SALT_BCD_SIZE ) == NULL )
		throw std::logic_error( "PasswordSalt::toBCD() cannot convert hash ?!?" );

	return std::string( buffer );
}

std::string PasswordSalt::toBase64() const
{
	char	buffer[ PASSWORD_SALT_BASE64_SIZE ];

	if ( base64::encode( m_salt, PASSWORD_SALT_SIZE,
			     buffer, PASSWORD_SALT_BASE64_SIZE, 0 ) < 0 )
		throw std::logic_error( "PasswordSalt::toBase64() cannot convert hash ?!?" );

	return std::string( buffer );
}


PasswordHash::PasswordHash( const PasswordSalt& pwdSalt, const std::string& password )
	: m_salt( pwdSalt )
{
	sha224((const unsigned char*)password.c_str(), password.length(), m_hash );
}

std::string PasswordHash::toString() const
{
	char	buffer[ PASSWORD_HASH_BASE64_SIZE ];

	if ( base64::encode( m_hash, PASSWORD_HASH_SIZE,
			     buffer, PASSWORD_HASH_BASE64_SIZE, 0 ) < 0 )
		throw std::logic_error( "PasswordHash::toString() cannot convert hash ?!?" );

	std::string s = "$";
	s += m_salt.toBase64() + "$" + buffer;
	return s;
}
