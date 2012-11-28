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
/// HMAC-SHA256 C++ interface
///

#include <cstring>
#include <cassert>
#include <stdexcept>

#include "types/HMAC.hpp"
#include "types/HMAC.h"

#include "types/byte2hex.h"
#include "types/base64.hpp"

namespace _Wolframe {
namespace AAAA {

static const size_t HMAC_SHA256_BCD_SIZE = HMAC_SHA256_DIGEST_SIZE * 2 + 1;
static const size_t HMAC_SHA256_BASE64_SIZE = (( HMAC_SHA256_DIGEST_SIZE - 1 ) / 3 ) * 4 + 5;

HMAC_SHA256::HMAC_SHA256( const std::string& digest )
{
	memset( m_HMAC, 0, HMAC_SHA256_DIGEST_SIZE );

	int ret;
	if (( ret = base64_decode( digest.data(), digest.size(),
				   m_HMAC, HMAC_SHA256_DIGEST_SIZE )) < 0 )	{
		std::string errMsg = "Cannot convert '" + digest + "' to a HMAC-SHA256";
		throw std::runtime_error( errMsg );
	}
}


std::string HMAC_SHA256::toBCD() const
{
	char	buffer[ HMAC_SHA256_BCD_SIZE ];

	int len = byte2hex( m_HMAC, HMAC_SHA256_DIGEST_SIZE, buffer, HMAC_SHA256_BCD_SIZE );
	assert( len == HMAC_SHA256_DIGEST_SIZE * 2 );

	return std::string( buffer );
}

std::string HMAC_SHA256::toString() const
{
	char	buffer[ HMAC_SHA256_BASE64_SIZE ];

	int len = base64::encode( m_HMAC, HMAC_SHA256_DIGEST_SIZE, buffer, HMAC_SHA256_BASE64_SIZE, 0 );
	assert( len > 0 && len < (int)HMAC_SHA256_BASE64_SIZE );

	while ( len > 0 && buffer[ len - 1 ] == '=' )
		len--;
	buffer[ len ] = 0;

	return std::string( buffer );
}

bool HMAC_SHA256::operator == ( const HMAC_SHA256& rhs ) const
{
	return !memcmp( this->m_HMAC, rhs.m_HMAC, HMAC_SHA256_DIGEST_SIZE );
}

bool HMAC_SHA256::operator == ( const std::string& rhs ) const
{
	unsigned char	buffer[ HMAC_SHA256_DIGEST_SIZE ];

	memset( buffer, 0, HMAC_SHA256_DIGEST_SIZE );
	if ( base64_decode( rhs.data(), rhs.size(), buffer, HMAC_SHA256_DIGEST_SIZE ) < 0 )
		return false;
	return !memcmp( this->m_HMAC, buffer, HMAC_SHA256_DIGEST_SIZE );
}

}} // namespace _Wolframe::AAAA
