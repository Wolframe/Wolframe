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
// Text file authentication
//

#include <string>
#include <cassert>

#include "logger-v1.hpp"
#include "TextFileAuth.hpp"
#include "crypto/sha2.h"
#include "types/byte2hex.h"
#include "AAAA/CRAM.hpp"
#include "AAAA/passwordHash.hpp"
#include "passwdFile.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

namespace _Wolframe {
namespace AAAA {

static const std::string AUTHENTICATION_MECH = "WOLFRAME-CRAM";

// Text file authentication - authentication unit
//***********************************************************************

const std::string TextFileAuthUnit::m_mechs[] = { AUTHENTICATION_MECH, "" };

TextFileAuthUnit::TextFileAuthUnit( const std::string& Identifier,
					      const std::string& filename )
	: AuthenticationUnit( Identifier ), m_pwdFile( filename, false )
{
	LOG_DEBUG << "Text file authenticator '" << identifier()
		      << "' created with file '" << m_pwdFile.filename() << "'";
}

TextFileAuthUnit::~TextFileAuthUnit()
{
}

AuthenticatorSlice* TextFileAuthUnit::slice( const std::string& /*mech*/,
					     const net::RemoteEndpoint& /*client*/ )
{
	return new TextFileAuthSlice( *this );
}

// clang says unused
//static const std::size_t PWD_LINE_SIZE = 1024;

User* TextFileAuthUnit::authenticatePlain( const std::string& username,
					   const std::string& password,
					   bool caseSensitveUser ) const
{
	PwdFileUser	user;

	if ( m_pwdFile.getUser( username, user, caseSensitveUser ))	{
		PasswordHash	filePwd( user.hash );
		PasswordHash	clientPwd( filePwd.salt(), password );

		PasswordHash::Hash	h0 = filePwd.hash();
		PasswordHash::Hash	h1 = clientPwd.hash();
		if ( h0 == h1 )
			return new User( "TextFile", user.user, user.info );
	}
	return NULL;
}


PwdFileUser TextFileAuthUnit::getUser( const std::string& hash, const std::string& key, PwdFileUser& user,
					    bool caseSensitveUser ) const
{
	if ( m_pwdFile.getHMACuser( hash, key, user, caseSensitveUser ))
		assert( !user.user.empty() );
	else
		user.user.clear();
	return user;
}


// Text file authentication - authentication slice
//***********************************************************************
TextFileAuthSlice::TextFileAuthSlice( const TextFileAuthUnit& backend )
	: m_backend( backend )
{
	m_user = NULL;
}

TextFileAuthSlice::~TextFileAuthSlice()
{
	if ( m_user != NULL )
		delete m_user;
}

void TextFileAuthSlice::destroy()
{
	delete this;
}

/// The input message
void TextFileAuthSlice::messageIn( const std::string& /*message*/ )
{
}

/// The output message
const std::string& TextFileAuthSlice::messageOut()
{
	return "";
}

/// The current status of the authenticator slice
AuthenticatorSlice::Status TextFileAuthSlice::status() const
{
}

/// The authenticated user or NULL if not authenticated
User* TextFileAuthSlice::user()
{
	return NULL;
}

}} // namespace _Wolframe::AAAA

