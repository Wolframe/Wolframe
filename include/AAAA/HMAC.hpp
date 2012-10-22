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

#ifndef _HMAC_HPP_INCLUDED
#define _HMAC_HPP_INCLUDED

#include <string>

#include "types/sha2.h"

namespace _Wolframe {
namespace AAAA {

static const size_t HMAC_DIGEST_SIZE = SHA256_DIGEST_SIZE;

class HMAC_SHA256
{
public:
	HMAC_SHA256( const unsigned char* key, size_t keySize,
		     const unsigned char* msg, size_t msgSize )
					{ init( key, keySize, msg, msgSize ); }
	HMAC_SHA256( const std::string& key, const std::string& message )
					{ init( (const unsigned char*)key.data(), key.size(),
						(const unsigned char*)message.data(), message.size()); }
	HMAC_SHA256( const unsigned char* key, size_t keySize, const std::string& message )
					{ init( key, keySize, (const unsigned char*)message.data(), message.size()); }
	HMAC_SHA256( const std::string& key, const unsigned char* msg, size_t msgSize )
					{ init( (const unsigned char*)key.data(), key.size(), msg, msgSize ); }
	HMAC_SHA256( const std::string& hash );

	bool operator == ( const HMAC_SHA256& rhs ) const;
	bool operator != ( const HMAC_SHA256& rhs ) const	{ return !( *this == rhs ); }

	///\note The string is a BCD representation of the value
	bool operator == ( const std::string& rhs ) const;
	bool operator != ( const std::string& rhs ) const	{ return !( *this == rhs ); }

	std::string toBCD() const;
	std::string toBase64() const;
private:
	void init( const unsigned char* key, size_t keyLen,
		   const unsigned char* msg, size_t msgLen );

	unsigned char	m_HMAC[ HMAC_DIGEST_SIZE ];
};

}} // namespace _Wolframe::AAAA

#endif // _HMAC_HPP_INCLUDED
