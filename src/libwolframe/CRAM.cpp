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
/// \file CRAM.cpp
/// Challenge Response Authentication Mechanism Implementation
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

#include "AAAA/CRAM.hpp"
#include "AAAA/password.hpp"
#include "types/byte2hex.h"
#include "types/base64.hpp"
#include "crypto/sha2.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe::AAAA;
using namespace _Wolframe;

static const size_t CRAM_CHALLENGE_BCD_SIZE = 2 * CRAM_CHALLENGE_SIZE + 1;
static const size_t CRAM_CHALLENGE_BASE64_SIZE = (( CRAM_CHALLENGE_SIZE - 1 ) / 3 ) * 4 + 5;

static const size_t CRAM_RESPONSE_BCD_SIZE = 2 * CRAM_RESPONSE_SIZE + 1;
static const size_t CRAM_RESPONSE_BASE64_SIZE = (( CRAM_RESPONSE_SIZE - 1 ) / 3 ) * 4 + 5;


CRAMchallenge::CRAMchallenge( const unsigned char* data, size_t bytes )
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

	memcpy( m_challenge + SHA256_DIGEST_SIZE, data,
		bytes > ( CRAM_CHALLENGE_SIZE - SHA256_DIGEST_SIZE ) ? CRAM_CHALLENGE_SIZE - SHA256_DIGEST_SIZE : bytes );
}

std::string CRAMchallenge::toBCD() const
{
	char	buffer[ CRAM_CHALLENGE_BCD_SIZE ];

	int len = byte2hex( m_challenge, CRAM_CHALLENGE_SIZE,
			    buffer, CRAM_CHALLENGE_BCD_SIZE );
	assert( len == CRAM_CHALLENGE_SIZE * 2 );

	return std::string( buffer );
}


std::string CRAMchallenge::toString() const
{
	char	buffer[ CRAM_CHALLENGE_BASE64_SIZE ];

	int len = base64_encode( m_challenge, CRAM_CHALLENGE_SIZE,
				 buffer, CRAM_CHALLENGE_BASE64_SIZE, 0 );
	assert( len >= 0 && len < (int)CRAM_CHALLENGE_BASE64_SIZE );
	while ( len > 0 && buffer[ len - 1 ] == '=' )
		len--;
	buffer[ len ] = 0;
	return std::string( buffer );
}


std::string CRAMchallenge::toString( const PasswordHash::Salt& salt ) const
{
	return "$" + salt.toString() + "$" + this->toString();
}

//***** CRAM response computation *******************************************
static void computeResponse ( const unsigned char* challenge, const unsigned char* hash,
			      std::size_t hashSize, unsigned char* response )
{
	unsigned char buffer[ CRAM_CHALLENGE_SIZE ];

	memset( buffer, 0x3c, CRAM_CHALLENGE_SIZE );
	if ( hashSize > CRAM_CHALLENGE_SIZE )	{
		assert( CRAM_CHALLENGE_SIZE == SHA512_DIGEST_SIZE );
		sha512( hash, hashSize, buffer );
	}
	else
		memcpy( buffer, hash, hashSize );

	for ( size_t i = 0; i < CRAM_CHALLENGE_SIZE; i++ )
		buffer[ i ] ^= challenge[ i ];

	assert( CRAM_RESPONSE_SIZE == SHA256_DIGEST_SIZE );
	sha256( buffer, CRAM_CHALLENGE_SIZE, response );
}
//***** End of CRAM response computation ************************************


CRAMresponse::CRAMresponse( const CRAMchallenge& challenge, const PasswordHash& pwdHash )
{
	computeResponse( challenge.challenge(), pwdHash.hash().hash(), pwdHash.hash().size(), m_response );
}

// The challenge string is base64 encoded, including the password salt,
// with or without end padding.
// The password string is in plain text.
CRAMresponse::CRAMresponse( const std::string& challenge, const std::string& password )
{
	unsigned char chlng[ CRAM_CHALLENGE_SIZE ];
	memset( chlng, 0, CRAM_CHALLENGE_SIZE );
	int chlngSize;
	unsigned char salt[ PASSWORD_SALT_SIZE ];
	memset( salt, 0, PASSWORD_SALT_SIZE );
	int saltSize;

	std::string s = boost::algorithm::trim_copy( challenge );
	if ( s[ 0 ] == '$' )	{
		size_t chlngStart = s.find( "$", 1 );
		if ( chlngStart == s.npos )	{
			std::string errMsg = "'" + s + "' is not a valid challenge string";
			throw std::runtime_error( errMsg );
		}
		if (( saltSize = base64::decode( s.substr( 1, chlngStart - 1 ),
						 salt, PASSWORD_SALT_SIZE )) < 0 )	{
			std::string errMsg = "'" + s + "' is not a valid challenge string (salt error)";
			throw std::runtime_error( errMsg );
		}
		if (( chlngSize = base64::decode( s.substr( chlngStart ),
						 salt, CRAM_CHALLENGE_SIZE )) < 0 )	{
			std::string errMsg = "'" + s + "' is not a valid challenge string (random data error)";
			throw std::runtime_error( errMsg );
		}
		if ( chlngSize != CRAM_CHALLENGE_SIZE )	{
			std::string errMsg = "'" + s + "' is not a valid challenge string (expected "
					     + boost::lexical_cast<std::string>( CRAM_CHALLENGE_SIZE )
					     + " random bytes, got "
					     + boost::lexical_cast<std::string>( chlngSize ) + ")";
			throw std::runtime_error( errMsg );
		}
	}
	else	{
		std::string errMsg = "'" + s + "' is not a valid challenge string";
		throw std::runtime_error( errMsg );
	}

	PasswordHash pwd( salt, saltSize, password );

	computeResponse( chlng, pwd.hash().hash(), pwd.hash().size(), m_response );
}


std::string CRAMresponse::toBCD() const
{
	char	buffer[ CRAM_RESPONSE_BCD_SIZE ];

	int len = byte2hex( m_response, CRAM_RESPONSE_SIZE,
			    buffer, CRAM_RESPONSE_BCD_SIZE );
	assert( len == CRAM_RESPONSE_SIZE * 2 );

	return std::string( buffer );
}

std::string CRAMresponse::toString() const
{
	char	buffer[ CRAM_RESPONSE_BASE64_SIZE ];

	int len = base64_encode( m_response, CRAM_RESPONSE_SIZE,
				 buffer, CRAM_RESPONSE_BASE64_SIZE, 0 );
	assert( len >= 0 && len < (int)CRAM_RESPONSE_BASE64_SIZE );
	while ( len > 0 && buffer[ len - 1 ] == '=' )
		len--;
	buffer[ len ] = 0;
	return std::string( buffer );
}


bool CRAMresponse::operator == ( const CRAMresponse& rhs )
{
	return !memcmp( this->m_response, rhs.m_response, CRAM_RESPONSE_SIZE );
}

bool CRAMresponse::operator == ( const std::string& rhs )
{
	unsigned char	buffer[ CRAM_RESPONSE_SIZE ];

	if ( base64_decode( rhs.data(), rhs.size(),
			    buffer, CRAM_RESPONSE_SIZE ) != (int)CRAM_RESPONSE_SIZE )
		return false;
	return !memcmp( this->m_response, buffer, CRAM_RESPONSE_SIZE );
}

