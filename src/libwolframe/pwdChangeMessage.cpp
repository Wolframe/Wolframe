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
#include <stdexcept>
#include <string.h>
#include <boost/lexical_cast.hpp>

namespace _Wolframe {
namespace AAAA {

PasswordChangeMessage::PasswordChangeMessage( const std::string& pwd )
{
	memset( data, 0, 64 );
	if ( pwd.length() > PASSWORD_MAX_LENGTH )	{
		std::string msg = "Password is " + boost::lexical_cast< std::string >( pwd.length())
				  + "bytes long, maximum is "
				  + boost::lexical_cast< std::string >( PASSWORD_MAX_LENGTH ) + " bytes";
		throw( std::runtime_error( msg ) );
	}
	message.length = pwd.length();
	memcpy( message.passwd, pwd.data(), PASSWORD_MAX_LENGTH );
	md5_ctx ctx;
	md5_init( &ctx );
	md5( data, sizeof(unsigned short) + PASSWORD_MAX_LENGTH, message.digest );
}

PasswordChangeMessage::PasswordChangeMessage( const unsigned char msg[ 64 ] )
{
	unsigned char hash[ MD5_DIGEST_SIZE ];

	memcpy( data, msg, 64 );

	md5_ctx ctx;
	md5_init( &ctx );
	md5( data, sizeof(unsigned short) + PASSWORD_MAX_LENGTH, hash );

	if ( memcmp( message.digest, hash, 16 ))
		throw( std::runtime_error( "Message is not a valid password change message" ));
}

std::string PasswordChangeMessage::password() const
{
	std::string pwd( message.passwd, message.length );
	return pwd;
}

}} // namespace _Wolframe::AAAA
