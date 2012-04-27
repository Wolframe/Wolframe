/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
// Challenge Response Authentication Mechanism classes
//

#ifndef _CRAM_HPP_INCLUDED
#define _CRAM_HPP_INCLUDED

#include <string>
#include <sys/types.h>

namespace _Wolframe {
namespace AAAA {

static const ssize_t CRAM_BLOCK_SIZE = 1024 / 8;
static const ssize_t CRAM_DIGEST_SIZE = 512 / 8;
static const ssize_t PASSWORD_DIGEST_SIZE = 224 / 8;
static const ssize_t CRAM_CHALLENGE_SIZE = CRAM_BLOCK_SIZE;
static const ssize_t CRAM_RESPONSE_SIZE = CRAM_DIGEST_SIZE;

class PasswordHash
{
public:
	PasswordHash( const std::string& password );
	std::string toString();
private:
	unsigned char	m_hash[ PASSWORD_DIGEST_SIZE ];
};


class CRAMchallenge
{
	friend class CRAMresponse;
public:
	CRAMchallenge( const std::string& randomDevice );
	std::string toString();
private:
	unsigned char	m_challenge[ CRAM_CHALLENGE_SIZE ];
};


class CRAMresponse
{
public:
	CRAMresponse(const CRAMchallenge& challenge,
		     const std::string& username, const std::string &pwdHash);
	CRAMresponse( const std::string& challenge,
		      const std::string& username, const std::string& pwdHash );

	std::string toString();
	bool operator == ( const CRAMresponse& rhs );
	bool operator != ( const CRAMresponse& rhs )	{ return !( *this == rhs ); }
private:
	unsigned char	m_response[ CRAM_RESPONSE_SIZE ];
};

}} // namespace _Wolframe::AAAA

#endif	// _CRAM_HPP_INCLUDED

