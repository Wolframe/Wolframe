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
/// \file AAAA/usernameHash.hpp
/// \brief username hash used by Wolframe authentication

#ifndef _USERNAME_HASH_HPP_INCLUDED
#define _USERNAME_HASH_HPP_INCLUDED

#include <string>
#include <system/randomGenerator.hpp>
#include "crypto/HMAC.h"

namespace _Wolframe {
namespace AAAA {

static const size_t USERNAME_HASH_SIZE = HMAC_SHA256_HASH_SIZE;
static const size_t USERNAME_HASH_SALT_SIZE = 128 / 8;

class UsernameHash
{
public:
	UsernameHash( const crypto::RandomGenerator& rndGen, const std::string& username );
	UsernameHash( const unsigned char *slt, std::size_t sltSize,
		      const std::string& username );
	UsernameHash( const std::string& hashString );

	~UsernameHash();

	unsigned char* salt()				{ return m_salt; }
	std::size_t saltSize() const			{ return m_saltSize; }
	unsigned char* hash()				{ return m_hash; }
	std::size_t hashSize() const			{ return sizeof( m_hash ); }

	/// Return a base64 string in the format $<salt>$<hash>
	std::string toString() const;

	/// Return a BCD string in the format <salt>:<hash>
	std::string toBCD() const;

	/// True if the 2 user hashes are identical, false otherwise.
	bool operator == ( const UsernameHash& rhs );
	bool operator != ( const UsernameHash& rhs )	{ return !( *this == rhs ); }

	/// True if the hash base64 encoding is equivalent to the given argument,
	/// false otherwise.
	/// \note The strings are base64 encoded, with or without end padding.
	bool operator == ( const std::string& rhs )	{ return *this == UsernameHash( rhs ); }
	bool operator != ( const std::string& rhs )	{ return !( *this == rhs ); }

private:
	std::size_t	m_saltSize;
	unsigned char	m_salt[ USERNAME_HASH_SALT_SIZE ];
	unsigned char	m_hash[ USERNAME_HASH_SIZE ];
};

}} // namespace _Wolframe::AAAA

#endif	// _USERNAME_HASH_HPP_INCLUDED

