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
/// \file CRAM.hpp
/// Challenge Response Authentication Mechanism classes
///
/// \note This implementation of a CRAM mechanism is specific to the Wolframe Project.
/// It is not supposed to be compatible to other implementations.
/// But we hope that is at least as secure as any other authentication implementation.
//

#ifndef _CRAM_HPP_INCLUDED
#define _CRAM_HPP_INCLUDED

#include <string>

namespace _Wolframe {
namespace AAAA {

static const size_t CRAM_BLOCK_SIZE = 512 / 8;
static const size_t CRAM_DIGEST_SIZE = 256 / 8;
static const size_t CRAM_CHALLENGE_SIZE = CRAM_BLOCK_SIZE;
static const size_t CRAM_RESPONSE_SIZE = CRAM_DIGEST_SIZE;

class CRAMchallenge
{
public:
	CRAMchallenge( const unsigned char* data, size_t bytes );

	const unsigned char* challenge() const		{ return m_challenge; }
	std::size_t size() const			{ return CRAM_CHALLENGE_SIZE; }

	std::string toBCD() const;
	std::string toString() const;
private:
	unsigned char	m_challenge[ CRAM_CHALLENGE_SIZE ];
};


class CRAMresponse
{
public:
	/// Constructor
	/// \note The strings are base64 encoded, with or without end padding.
	CRAMresponse( const CRAMchallenge& challenge,
		      const unsigned char *hash, std::size_t hashSize );
	CRAMresponse( const std::string& challenge,
		      const unsigned char *hash, std::size_t hashSize  );
	CRAMresponse( const CRAMchallenge& challenge, const std::string& pwdHash );
	CRAMresponse( const std::string& challenge, const std::string& pwdHash );

	const unsigned char* response() const		{ return m_response; }
	std::size_t size() const			{ return CRAM_RESPONSE_SIZE; }

	std::string toBCD() const;
	std::string toString() const;

	/// True if the 2 CRAM responses are identical, false otherwise.
	bool operator == ( const CRAMresponse& rhs );
	bool operator != ( const CRAMresponse& rhs )	{ return !( *this == rhs ); }

	/// True if the CRAM response base64 encoding is equivalent to the given argument,
	/// false otherwise.
	/// \note The strings are base64 encoded, with or without end padding.
	bool operator == ( const std::string& rhs );
	bool operator != ( const std::string& rhs )	{ return !( *this == rhs ); }
private:
	unsigned char	m_response[ CRAM_RESPONSE_SIZE ];
};

}} // namespace _Wolframe::AAAA

#endif	// _CRAM_HPP_INCLUDED

