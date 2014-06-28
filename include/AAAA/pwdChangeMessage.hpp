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
///
/// Password changer data structure

#ifndef _PASSWORD_CHANGE_MESSAGE_HPP_INCLUDED
#define _PASSWORD_CHANGE_MESSAGE_HPP_INCLUDED

#include <string>
#include "crypto/md5.h"

namespace _Wolframe {
namespace AAAA {

#define PASSWORD_MAX_LENGTH	( 64 - sizeof( unsigned short ) - MD5_DIGEST_SIZE )


/// Password changer data structure
union PasswordChangeMessage
{
	unsigned char data[ 64 ];
	struct message	{
		unsigned short	length;
		unsigned char	password[ PASSWORD_MAX_LENGTH ];
		unsigned char	digest[ MD5_DIGEST_SIZE ];
	};

public:
	PasswordChangeMessage( const std::string pwd );
	PasswordChangeMessage( const unsigned char message[ 64 ] );

	std::string password() const;
};

}} // namespace _Wolframe::AAAA

#endif // _PASSWORD_CHANGE_MESSAGE_HPP_INCLUDED
