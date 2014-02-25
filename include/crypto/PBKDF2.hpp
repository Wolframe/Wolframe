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
/// Standard PBKDF2-HMAC-SHA1 C++ interface
///

#ifndef _PBKDF2_HPP_INCLUDED
#define _PBKDF2_HPP_INCLUDED

#include <string>

namespace _Wolframe {
namespace crypto {

class PBKDF2_HMAC_SHA1
{
public:
	PBKDF2_HMAC_SHA1( const unsigned char* key, size_t keySize,
			  const unsigned char* password, size_t pwdSize,
			  std::size_t dkLen, unsigned int rounds );
	PBKDF2_HMAC_SHA1( const std::string& key, const std::string& message,
			  std::size_t dkLen, unsigned int rounds );
	PBKDF2_HMAC_SHA1( const unsigned char* key, size_t keySize, const std::string& message,
			  std::size_t dkLen, unsigned int rounds );
	PBKDF2_HMAC_SHA1( const std::string& key, const unsigned char* msg, size_t msgSize,
			  std::size_t dkLen, unsigned int rounds );

	/// \note The string is a base64 representation of the value
	PBKDF2_HMAC_SHA1( const std::string& str );

	~PBKDF2_HMAC_SHA1();

	const unsigned char* hash() const			{ return m_hash; }
	std::size_t size() const				{ return m_dkLen; }

	/// Comparisson operators
	bool operator == ( const PBKDF2_HMAC_SHA1& rhs ) const;
	bool operator != ( const PBKDF2_HMAC_SHA1& rhs ) const	{ return !( *this == rhs ); }

	/// Comparisson operators
	/// \note The string is a base64 representation of the value
	bool operator == ( const std::string& rhs ) const;
	bool operator != ( const std::string& rhs ) const	{ return !( *this == rhs ); }

	/// BCD string representation of the PBKDF2-HMAC-SHA1 value.
	std::string toBCD() const;
	/// Base64 string representation of the PBKDF2-HMAC-SHA1 value.
	/// \note The string is without the base64 end padding
	std::string toString() const;
private:
	std::size_t	m_dkLen;
	unsigned char*	m_hash;
};

}} // namespace _Wolframe::crypto

#endif // _PBKDF2_HPP_INCLUDED
