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
/// \file CRAM.hpp
/// Challenge Response Authentication Mechanism classes
///
/// \note This implementation of a CRAM mechanism is specific to the Wolframe Project.
/// It is not supposed to be compatible to any other implementation.
/// But we hope that is at least as secure as any other authentication implementation.
//

#ifndef _CRAM_HPP_INCLUDED
#define _CRAM_HPP_INCLUDED

#include <string>

namespace _Wolframe {
namespace AAAA {

static const int CRAM_BLOCK_SIZE = 1024 / 8;
static const int CRAM_DIGEST_SIZE = 512 / 8;
static const int PASSWORD_DIGEST_SIZE = 224 / 8;
static const int PASSWORD_SALT_SIZE = 128 / 8;
static const int CRAM_CHALLENGE_SIZE = CRAM_BLOCK_SIZE;
static const int CRAM_RESPONSE_SIZE = CRAM_DIGEST_SIZE;


class PasswordSalt
{
	static const std::size_t PASSWORD_SALT_SIZE = 128 / 8;
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
	PasswordHash( const PasswordSalt& pwdSalt, const std::string& password );
	PasswordHash( const std::string& hash );

	std::string toBCD() const;
	std::string toBase64() const;

	const PasswordSalt& salt() const	{ return m_salt; }
private:
	unsigned char		m_hash[ PASSWORD_DIGEST_SIZE ];
	const PasswordSalt	m_salt;
};


class CRAMchallenge
{
	friend class CRAMresponse;
public:
	CRAMchallenge( const std::string& randomDevice );

	std::string toBCD() const;
	std::string toBase64() const;
private:
	unsigned char		m_challenge[ CRAM_CHALLENGE_SIZE ];
};


class CRAMresponse
{
public:
	CRAMresponse( const CRAMchallenge& challenge,
		      const std::string& username, const std::string &pwdHash );
	CRAMresponse( const std::string& challenge,
		      const std::string& username, const std::string& pwdHash );

	std::string toBCD() const;
	std::string toBase64() const;

	/// True if the 2 CRAM responses are identical, false otherwise
	bool operator == ( const CRAMresponse& rhs );
	bool operator != ( const CRAMresponse& rhs )	{ return !( *this == rhs ); }
	/// True if the CRAM response base64 encoding is equivalent to the given argument,
	/// false otherwise
	bool operator == ( const std::string& rhs );
	bool operator != ( const std::string& rhs )	{ return !( *this == rhs ); }
private:
	unsigned char		m_response[ CRAM_RESPONSE_SIZE ];
};

}} // namespace _Wolframe::AAAA

#endif	// _CRAM_HPP_INCLUDED

