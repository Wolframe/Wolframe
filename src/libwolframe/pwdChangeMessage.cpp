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
/// \file pwdChangeMessage.cpp
///
/// Password changer data structure implementation

#include "AAAA/pwdChangeMessage.hpp"
#include "crypto/md5.h"
#include "crypto/AES256.h"
#include "types/base64.hpp"
#include <stdexcept>
#include <string>
#include <cstring>
#include <boost/lexical_cast.hpp>

namespace _Wolframe {
namespace AAAA {

PasswordChangeMessage::PasswordChangeMessage( const std::string& pwd )
{
	memset( m_message.data, 0, 64 );
	if ( pwd.length() > PASSWORD_MAX_LENGTH )	{
		std::string msg = "Password is " + boost::lexical_cast< std::string >( pwd.length())
				  + "bytes long, maximum is "
				  + boost::lexical_cast< std::string >( PASSWORD_MAX_LENGTH ) + " bytes";
		throw( std::runtime_error( msg ) );
	}
	m_message.parts.length = pwd.length();
	memcpy( m_message.parts.passwd, pwd.data(), m_message.parts.length );
	md5_ctx ctx;
	md5_init( &ctx );
	md5( m_message.data, PAYLOAD_LENGTH, m_message.parts.digest );
}

PasswordChangeMessage::PasswordChangeMessage( const unsigned char msg[ 64 ] )
{
	if ( !isValid( msg, 64 ) )
		throw( std::runtime_error( "Message is not a valid password change message" ));
	memcpy( m_message.data, msg, 64 );
}


/// Check if the buffer is a valid message
bool PasswordChangeMessage::isValid( const unsigned char buffer[], std::size_t size )
{
	if ( size != 64u )
		return false;

	unsigned char hash[ MD5_DIGEST_SIZE ];
	md5_ctx ctx;
	md5_init( &ctx );
	md5( buffer, PAYLOAD_LENGTH, hash );
	if ( memcmp( buffer + PAYLOAD_LENGTH, hash, MD5_DIGEST_SIZE ))
		return false;
	return true;
}


// Encrypt the message to a base64 string
std::string PasswordChangeMessage::toBase64( const unsigned char IV[ 16 ], const unsigned char key[ 32 ] ) const
{
	unsigned char	crypted[ 64 ];
	memcpy( crypted, m_message.data, 64 );

	AES256_context	ctx;
	AES256_init( &ctx, key );
	AES256_encrypt_CBC( &ctx, IV, crypted, 64 );
	AES256_done( &ctx );

	return base64::encode( crypted, 64, 0 );
}

// Build the message from an encrypted base64 message
bool PasswordChangeMessage::fromBase64( const std::string& msg,
					const unsigned char IV[ 16 ], const unsigned char key[ 32 ] )
{
	unsigned char	crypted[ 64 ];
	if ( base64::decode( msg, crypted, 64 ) < 0 )
		return false;

	AES256_context	ctx;
	AES256_init( &ctx, key );
	AES256_decrypt_CBC( &ctx, IV, crypted, 64 );
	AES256_done( &ctx );

	if ( !isValid( crypted ))
		return false;
	memcpy( m_message.data, crypted, 64 );
	return true;
}

}} // namespace _Wolframe::AAAA
