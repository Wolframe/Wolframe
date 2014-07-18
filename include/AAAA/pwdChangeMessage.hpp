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
/// \file pwdChangeMessage.hpp
/// \brief Password changer data structure

#ifndef _PASSWORD_CHANGE_MESSAGE_HPP_INCLUDED
#define _PASSWORD_CHANGE_MESSAGE_HPP_INCLUDED

#include <string>
#include <cstring>
#include "crypto/md5.h"

namespace _Wolframe {
namespace AAAA {

/// Password changer data structure
class PasswordChangeMessage
{
private:
	enum	{
		PASSWORD_MAX_LENGTH = 64 - sizeof( unsigned char ) - MD5_DIGEST_SIZE,
		PAYLOAD_LENGTH = 64 - MD5_DIGEST_SIZE
	};

	union {
		unsigned char data[ 64 ];
		struct	{
			unsigned char	length;
			char		passwd[ PASSWORD_MAX_LENGTH ];
			unsigned char	digest[ MD5_DIGEST_SIZE ];
		} parts;
	} m_message;

public:
	PasswordChangeMessage( const std::string& pwd );
	PasswordChangeMessage( const unsigned char msg[ 64 ] );

	/// Empty message constructor
	/// \note that this is an invalid message
	PasswordChangeMessage()			{ memset( m_message.data, 0, 64 ); }

	~PasswordChangeMessage()		{ clear(); }

	void clear()				{ memset( m_message.data, 0, 64 ); }
	unsigned char* data()			{ return m_message.data; }
	std::size_t size() const		{ return sizeof( m_message.data ); }
	std::string password() const		{ return std::string( m_message.parts.passwd,
								      m_message.parts.length ); }

	/// Check if the buffer is a valid message
	static bool isValid( const unsigned char buffer[ 64 ] )
						{ return isValid( buffer, 64u ); }
	static bool isValid( const unsigned char buffer[], std::size_t size );

	/// Check if the message is valid
	bool isValid() const			{ return isValid( m_message.data, 64u ); }

	/// Encrypt the message to a base64 string
	std::string toBase64( const unsigned char IV[ 16 ], const unsigned char key[ 32 ] ) const;
	/// Build the message from an encrypted base64 message
	bool fromBase64( const std::string& msg,
			 const unsigned char IV[ 16 ], const unsigned char key[ 32 ] );
};

}} // namespace _Wolframe::AAAA

#endif // _PASSWORD_CHANGE_MESSAGE_HPP_INCLUDED
