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
/// \file password.hpp
/// Wolframe password hashes
///

#ifndef _PASSWORD_HPP_INCLUDED
#define _PASSWORD_HPP_INCLUDED

#include <string>

namespace _Wolframe {
namespace AAAA {

static const size_t PASSWORD_HASH_SIZE = 224 / 8;
static const size_t PASSWORD_SALT_SIZE = 128 / 8;

class PasswordSalt
{
	friend class PasswordHash;
public:
	/// Construct an empty salt (all bits 0).
	PasswordSalt();
	/// Construct a salt from the given byte array.
	explicit PasswordSalt( const unsigned char* salt, size_t bytes );
	/// Construct a random salt using randomDevice to generate random bytes.
	explicit PasswordSalt( const std::string& randomDevice );

	/// The size of the salt in bytes
	size_t size() const			{ return m_size; }

	/// Set the salt value from the BCD encoded string.
	void fromBCD( const std::string& salt );
	/// Set the salt value from the base64 encoded string.
	void fromBase64( const std::string& salt );

	/// Return the salt as a BCD encoded string.
	std::string toBCD() const;
	/// Return the salt as a base64 encoded string.
	std::string toBase64() const;
private:
	std::size_t	m_size;
	unsigned char	m_salt[ PASSWORD_SALT_SIZE ];
};


class PasswordHash
{
public:
	/// Construct the password hash from salt and password (plain text)
	PasswordHash( const PasswordSalt& pwdSalt, const std::string& password );
	/// Construct the password hash from a combined password hash string (base64)
	PasswordHash( const std::string& hash );

	/// Return the password hash as a string
	/// The format is $<salt>$<hash> on one line, no whitespaces
	std::string toString() const;

	const PasswordSalt& salt() const	{ return m_salt; }
private:
	unsigned char		m_hash[ PASSWORD_HASH_SIZE ];
	const PasswordSalt	m_salt;
};

}} // namespace _Wolframe::AAAA

#endif	// _PASSWORD_HPP_INCLUDED

