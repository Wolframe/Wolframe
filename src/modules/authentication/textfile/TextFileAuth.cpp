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
// Text file authentication
//

#include <string>

#include <cstdio>
#include <cerrno>

#include "logger-v1.hpp"
#include "TextFileAuth.hpp"
#include "types/sha2.h"
#include "types/byte2hex.h"
#include "AAAA/CRAM.hpp"
#include "AAAA/password.hpp"
#include "passwdFile.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "utils/miscUtils.hpp"

namespace _Wolframe {
namespace AAAA {

// Text file authentication - authentication unit
//***********************************************************************
TextFileAuthenticator::TextFileAuthenticator( const std::string& Identifier,
					      const std::string& filename )
	: AuthenticationUnit( Identifier ), m_pwdFile( filename, false )
{
	MOD_LOG_DEBUG << "Text file authenticator '" << identifier()
		      << "' created with file '" << m_pwdFile.filename() << "'";
}

TextFileAuthenticator::~TextFileAuthenticator()
{
}

AuthenticatorInstance* TextFileAuthenticator::instance()
{
	return new TextFileAuthSlice( *this );
}


static const std::size_t PWD_LINE_SIZE = 1024;

User* TextFileAuthenticator::authenticatePlain( const std::string& username,
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


//User* TextFileAuthenticator::authenticate( const CRAMchallenge& challenge,
//					   const CRAMresponse& response,
//					   bool caseSensitveUser ) const
//{
//	return authenticate( challenge, response.toString(), caseSensitveUser );
//}

//User* TextFileAuthenticator::authenticate( const CRAMchallenge& challenge,
//					   const std::string& response,
//					   bool caseSensitveUser ) const
//{
//	PwdFileUser	user;

//	if ( m_pwdFile.getUser( username, user, caseSensitveUser ))	{
//		PasswordHash	filePwd( user.hash );
//		try	{
//			CRAMresponse	local( challenge, filePwd.hash() );
//			if ( local == response )
//				return new User( "TextFile", user.user, user.info );
//		}
//	}
//	return NULL;
//}


// Text file authentication - authentication slice
//***********************************************************************
TextFileAuthSlice::TextFileAuthSlice( const TextFileAuthenticator& backend )
	: m_backend( backend )
{
	m_user = NULL;
}

TextFileAuthSlice::~TextFileAuthSlice()
{
	if ( m_user != NULL )
		delete m_user;
}

void TextFileAuthSlice::receiveData( const void* /*data*/, std::size_t /*size*/ )
{
}

const FSM::Operation TextFileAuthSlice::nextOperation()
{
	FSM::Operation	op;
	return op;
}

void TextFileAuthSlice::signal( FSM::Signal /*event*/ )
{
}

std::size_t TextFileAuthSlice::dataLeft( const void*& /*begin*/ )
{
	return 0;
}

User* TextFileAuthSlice::user()
{
	User* ret = m_user;
	m_user = NULL;
	return ret;
}

}} // namespace _Wolframe::AAAA

