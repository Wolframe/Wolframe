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
/// \file password.hpp
/// Wolframe password hashes
///

#ifndef _PASSWORD_HPP_INCLUDED
#define _PASSWORD_HPP_INCLUDED

#include <string>
#include <system/randomGenerator.hpp>

namespace _Wolframe {
namespace AAAA {

static const size_t PASSWORD_HASH_SIZE = 384 / 8;
static const size_t PASSWORD_SALT_SIZE = 128 / 8;

class PasswordHash
{
public:
	class Salt
	{
		friend class PasswordHash;
	public:
		/// Construct an empty salt (all bits 0).
		Salt();
		/// Construct a random salt.
		Salt( const crypto::RandomGenerator& rndGen );
		/// Construct a salt from the given byte array.
		Salt( const unsigned char* data, size_t bytes );
		/// Construct a salt by setting the value from the base64 encoded string.
		Salt( const std::string& str );

		/// Destructor (set all bits 0).
		~Salt();

		/// True if the 2 password salts are identical, false otherwise.
		bool operator == ( const Salt& rhs );
		bool operator != ( const Salt& rhs )	{ return !( *this == rhs ); }

		/// The size of the salt in bytes
		size_t size() const			{ return m_size; }
		/// The unsigned char vector of the salt
		const unsigned char* salt() const	{ return m_salt; }

		/// Return the salt as a BCD encoded string.
		std::string toBCD() const;
		/// Return the salt as a base64 encoded string (without base64 padding).
		std::string toString() const;
	private:
		std::size_t	m_size;
		unsigned char	m_salt[ PASSWORD_SALT_SIZE ];
	};

	class Hash
	{
		friend class PasswordHash;
	public:
		/// Construct an empty password hash (all bits 0).
		Hash();
		/// Construct the password hash from the given byte array.
		Hash( const unsigned char* data, size_t bytes );
		/// Construct the password hash by setting the value from the base64 encoded string.
		Hash( const std::string& str );

		/// Destruct the password hash (set all bits 0).
		~Hash();

		/// True if the 2 password hashes are identical, false otherwise
		bool operator == ( const Hash& rhs );
		bool operator != ( const Hash& rhs )	{ return !( *this == rhs ); }

		/// The size of the password hash in bytes
		size_t size() const			{ return PASSWORD_HASH_SIZE; }
		/// The unsigned char vector of the password hash
		const unsigned char* hash() const	{ return m_hash; }

		/// Return the password hash as a BCD encoded string.
		std::string toBCD() const;
		/// Return the password hash as a base64 encoded string
		/// (without base64 padding).
		std::string toString() const;
	private:
		unsigned char	m_hash[ PASSWORD_HASH_SIZE ];
	};

	/// Construct an empty password hash.
	PasswordHash()			{}

	/// Construct the password hash with a random salt
	/// \param rndGen	the random number generator
	/// \param password	password as plain text
	PasswordHash( const crypto::RandomGenerator& rndGen, const std::string& password );

	/// Construct the password hash from salt and password (plain text)
	/// \param pwdSalt	password salt as bytes
	/// \param bytes	password salt length in bytes
	/// \param password	password as plain text
	PasswordHash( const unsigned char* pwdSalt, size_t bytes, const std::string& password );

	/// Construct the password hash from salt and password (plain text)
	/// \param pwdSalt	password salt as PasswordHash::Salt
	/// \param password	password as plain text
	PasswordHash( const Salt& pwdSalt, const std::string& password );

	/// Construct the password hash from salt and password (plain text)
	/// \param pwdSalt	password salt as a base64 string
	/// \param password	password as plain text
	PasswordHash( const std::string& pwdSalt, const std::string& password );

	/// Construct the password hash from a combined password hash string (base64)
	/// \param str	password hash string in format $<salt>$<hash>
	///		if the string doesn't start with '$' then
	///		the string is considered to represent only the
	///		password hash as base64 and the salt will be zeroed
	PasswordHash( const std::string& str );

	/// The password salt
	const Salt& salt() const	{ return m_salt; }

	/// The unsigned char vector of the password hash
	const Hash& hash() const	{ return m_hash; }

	/// Return the password hash as a BCD string.
	/// The format is $<salt>$<hash> on one line, no whitespaces.
	std::string toBCD() const;
	/// Return the password hash as a base64 string without base64 padding.
	/// The format is $<salt>$<hash> on one line, no whitespaces.
	std::string toString() const;

private:
	Hash	m_hash;
	Salt	m_salt;
};

}} // namespace _Wolframe::AAAA

#endif	// _PASSWORD_HPP_INCLUDED

