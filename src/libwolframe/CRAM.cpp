/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
// Challenge Response Authentication Mechanism Implementation
//

/**
 * @defgroup CRAM Challenge Response Authentication Mechanism functions
 * @ingroup miscFunctions
 * @{
 */

/**
 * @file CRAM.cpp
 * @brief Implementation of the Challenge Response Authentication Mechanism
 * @author Mihai Barbos <mihai.barbos@gmail.com>
 */

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

#include "AAAA/CRAM.hpp"
#include "byte2hex.h"
#include "sha2.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace _Wolframe::AAAA;

static const int PASSWORD_HASH_STRING_SIZE = 2 * PASSWORD_DIGEST_SIZE + 1;
static const int CRAM_CHALLENGE_STRING_SIZE = 2 * CRAM_CHALLENGE_SIZE + 1;
static const int CRAM_RESPONSE_STRING_SIZE = 2 * CRAM_RESPONSE_SIZE + 1;

PasswordHash::PasswordHash( const PasswordSalt& pwdSalt, const std::string& password )
	: m_salt( pwdSalt )
{
	sha224((const unsigned char*)password.c_str(), password.length(), m_hash );
}

std::string PasswordHash::toBCD() const
{
	char	buffer[ PASSWORD_HASH_STRING_SIZE ];

	memset( buffer, 0, PASSWORD_HASH_STRING_SIZE );

	if ( byte2hex( m_hash, PASSWORD_DIGEST_SIZE,
		       buffer, PASSWORD_HASH_STRING_SIZE ) == NULL )
		throw std::logic_error( "PasswordHash::toBCD() cannot convert hash ?!?" );

	return std::string( buffer );
}

std::string PasswordHash::toBase64() const
{
	char	buffer[ PASSWORD_HASH_STRING_SIZE ];

	memset( buffer, 0, PASSWORD_HASH_STRING_SIZE );

	if ( byte2hex( m_hash, PASSWORD_DIGEST_SIZE,
		       buffer, PASSWORD_HASH_STRING_SIZE ) == NULL )
		throw std::logic_error( "PasswordHash::toString() cannot convert hash ?!?" );

	return std::string( buffer );
}

CRAMchallenge::CRAMchallenge( const std::string& randomDevice )
{
	memset( m_challenge, 0, CRAM_CHALLENGE_SIZE );

#ifndef _WIN32
	boost::posix_time::ptime pt =
			boost::posix_time::ptime( boost::posix_time::microsec_clock::universal_time());
	sha256((const unsigned char*)&pt, sizeof( pt ), m_challenge );
#else
	FILETIME ft = { 0, 0 };
	LARGE_INTEGER li;

	GetSystemTimeAsFileTime( &ft );
	QueryPerformanceCounter( &li );

	ft.dwLowDateTime += li.QuadPart;

	sha256((const unsigned char *)&ft, sizeof( ft ), m_challenge );
#endif

#ifndef _WIN32
	int hndl = open( randomDevice.c_str(), O_RDONLY );
	if ( hndl < 0 )	{
		std::string errMsg = "Error opening '" + randomDevice + "': ";
		throw std::runtime_error( errMsg );
	}

	int rndPart = read( hndl, m_challenge + SHA256_DIGEST_SIZE, CRAM_CHALLENGE_SIZE - SHA256_DIGEST_SIZE );
	if ( rndPart < 0 )	{
		std::string errMsg = "Error reading '" + randomDevice + "'";
		throw std::runtime_error( errMsg );
	}
	else if ( rndPart < CRAM_CHALLENGE_SIZE - SHA256_DIGEST_SIZE )	{
		std::string errMsg = "Not enough bytes in '" + randomDevice + "' ?!?";
		throw std::logic_error( errMsg );
	}

	close( hndl );
#else
	HCRYPTPROV provider = 0;

	if( !CryptAcquireContext( &provider, 0, randomDevice.c_str( ), PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT ) ) {
		throw std::runtime_error( "Error opening cyrpto context" );
	}

	if( !CryptGenRandom( provider, CRAM_CHALLENGE_SIZE - SHA256_DIGEST_SIZE, static_cast<BYTE *>( m_challenge + SHA256_DIGEST_SIZE ) ) ) {
		CryptReleaseContext( provider, 0 );
		throw std::runtime_error( "Error generating random data " );
	}

	if( !CryptReleaseContext( provider, 0 ) ) {
		throw std::runtime_error( "Error closing cyrpto context" );
	}
#endif
}

std::string CRAMchallenge::toBCD() const
{
	char	buffer[ CRAM_CHALLENGE_STRING_SIZE ];

	memset( buffer, 0, CRAM_CHALLENGE_STRING_SIZE );

	if ( byte2hex( m_challenge, CRAM_CHALLENGE_SIZE,
		       buffer, CRAM_CHALLENGE_STRING_SIZE ) == NULL )
		throw std::logic_error( "PasswordHash::toString() cannot convert challenge ?!?" );

	return std::string( buffer );
}


CRAMresponse::CRAMresponse( const CRAMchallenge& challenge,
			    const std::string& username, const std::string& pwdHash )
{
	unsigned char buffer[ CRAM_CHALLENGE_SIZE ];

	memset( buffer, 0, CRAM_CHALLENGE_SIZE );
	memcpy( buffer, username.c_str(),
		CRAM_CHALLENGE_SIZE / 2 > username.length() ? username.length() : CRAM_CHALLENGE_SIZE / 2 );
	if ( hex2byte( pwdHash.c_str(), buffer + CRAM_CHALLENGE_SIZE / 2, CRAM_CHALLENGE_SIZE / 2 ) != PASSWORD_DIGEST_SIZE )
		throw std::runtime_error( "CRAM response: password hash is invalid" );
	for ( int i = 0; i < CRAM_CHALLENGE_SIZE; i++ )
		buffer[ i ] ^= challenge.m_challenge[ i ];
	sha512( buffer, CRAM_CHALLENGE_SIZE, m_response );
}

CRAMresponse::CRAMresponse( const std::string& challenge,
			    const std::string &username, const std::string& pwdHash )
{
	unsigned char chlng[ CRAM_CHALLENGE_SIZE ];
	unsigned char buffer[ CRAM_CHALLENGE_SIZE ];

	memset( buffer, 0, CRAM_CHALLENGE_SIZE );
	memcpy( buffer, username.c_str(),
		CRAM_CHALLENGE_SIZE / 2 > username.length() ? username.length() : CRAM_CHALLENGE_SIZE / 2 );
	if ( hex2byte( pwdHash.c_str(), buffer + CRAM_CHALLENGE_SIZE / 2, CRAM_CHALLENGE_SIZE / 2 ) != PASSWORD_DIGEST_SIZE )
		throw std::runtime_error( "CRAM response: password hash is invalid" );
	if ( hex2byte( challenge.c_str(), chlng, CRAM_CHALLENGE_SIZE ) != CRAM_CHALLENGE_SIZE )
		throw std::runtime_error( "CRAM response: challenge is invalid" );
	for ( int i = 0; i < CRAM_CHALLENGE_SIZE; i++ )
		buffer[ i ] ^= chlng[ i ];
	sha512( buffer, CRAM_CHALLENGE_SIZE, m_response );
}

std::string CRAMresponse::toBCD() const
{
	char	buffer[ CRAM_RESPONSE_STRING_SIZE ];

	memset( buffer, 0, CRAM_RESPONSE_STRING_SIZE );

	if ( byte2hex( m_response, CRAM_RESPONSE_SIZE,
		       buffer, CRAM_RESPONSE_STRING_SIZE ) == NULL )
		throw std::logic_error( "PasswordHash::toString() cannot convert response ?!?" );

	return std::string( buffer );
}

bool CRAMresponse::operator == ( const CRAMresponse& rhs )
{
	return !memcmp( this->m_response, rhs.m_response, CRAM_RESPONSE_SIZE );
}

bool CRAMresponse::operator == ( const std::string& rhs )
{
	unsigned char	buffer[ CRAM_RESPONSE_SIZE ];

	if ( hex2byte( rhs.c_str(), buffer, CRAM_RESPONSE_SIZE ) != CRAM_RESPONSE_SIZE )
		return false;
	return !memcmp( this->m_response, buffer, CRAM_RESPONSE_SIZE );
}

/** @} *//* CRAM */

