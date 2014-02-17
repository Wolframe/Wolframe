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
/// Standard HMAC-SHA256 C++ interface
///

#ifndef _HMAC_HPP_INCLUDED
#define _HMAC_HPP_INCLUDED

#include <string>
#include "HMAC.h"

namespace _Wolframe {
namespace crypto {

/// Standard HMAC-SHA1 object
class HMAC_SHA1
{
public:
	HMAC_SHA1( const unsigned char* key, size_t keySize,
		   const unsigned char* msg, size_t msgSize )
					{ hmac_sha1( key, keySize,
						     msg, msgSize, m_HMAC ); }
	HMAC_SHA1( const std::string& key, const std::string& message )
					{ hmac_sha1( (const unsigned char*)key.data(), key.size(),
						     (const unsigned char*)message.data(), message.size(),
						     m_HMAC ); }
	HMAC_SHA1( const unsigned char* key, size_t keySize, const std::string& message )
					{ hmac_sha1( key, keySize,
						     (const unsigned char*)message.data(), message.size(),
						     m_HMAC ); }
	HMAC_SHA1( const std::string& key, const unsigned char* msg, size_t msgSize )
					{ hmac_sha1( (const unsigned char*)key.data(), key.size(),
						     msg, msgSize, m_HMAC ); }

	/// \note The string is a base64 representation of the value
	HMAC_SHA1( const std::string& str );

	const unsigned char* hash() const			{ return m_HMAC; }
	std::size_t size() const				{ return HMAC_SHA1_HASH_SIZE; }

	/// Comparisson operators
	bool operator == ( const HMAC_SHA1& rhs ) const;
	bool operator != ( const HMAC_SHA1& rhs ) const	{ return !( *this == rhs ); }

	/// Comparisson operators
	/// \note The string is a base64 representation of the value
	bool operator == ( const std::string& rhs ) const;
	bool operator != ( const std::string& rhs ) const	{ return !( *this == rhs ); }

	/// BCD string representation of the HMAC value.
	std::string toBCD() const;
	/// Base64 string representation of the HMAC value.
	/// \note The string is without the base64 end padding
	std::string toString() const;
private:
	unsigned char	m_HMAC[ HMAC_SHA1_HASH_SIZE ];
};


/// Standard HMAC-SHA256 object
class HMAC_SHA256
{
public:
	HMAC_SHA256( const unsigned char* key, size_t keySize,
		     const unsigned char* msg, size_t msgSize )
					{ hmac_sha256( key, keySize,
						       msg, msgSize, m_HMAC ); }
	HMAC_SHA256( const std::string& key, const std::string& message )
					{ hmac_sha256( (const unsigned char*)key.data(), key.size(),
						       (const unsigned char*)message.data(), message.size(),
						       m_HMAC ); }
	HMAC_SHA256( const unsigned char* key, size_t keySize, const std::string& message )
					{ hmac_sha256( key, keySize,
						       (const unsigned char*)message.data(), message.size(),
						       m_HMAC ); }
	HMAC_SHA256( const std::string& key, const unsigned char* msg, size_t msgSize )
					{ hmac_sha256( (const unsigned char*)key.data(), key.size(),
						       msg, msgSize, m_HMAC ); }

	/// \note The string is a base64 representation of the value
	HMAC_SHA256( const std::string& str );

	const unsigned char* hash() const			{ return m_HMAC; }
	std::size_t size() const				{ return HMAC_SHA256_HASH_SIZE; }

	/// Comparisson operators
	bool operator == ( const HMAC_SHA256& rhs ) const;
	bool operator != ( const HMAC_SHA256& rhs ) const	{ return !( *this == rhs ); }

	/// Comparisson operators
	/// \note The string is a base64 representation of the value
	bool operator == ( const std::string& rhs ) const;
	bool operator != ( const std::string& rhs ) const	{ return !( *this == rhs ); }

	/// BCD string representation of the HMAC value.
	std::string toBCD() const;
	/// Base64 string representation of the HMAC value.
	/// \note The string is without the base64 end padding
	std::string toString() const;
private:
	unsigned char	m_HMAC[ HMAC_SHA256_HASH_SIZE ];
};

}} // namespace _Wolframe::crypto

#endif // _HMAC_HPP_INCLUDED
