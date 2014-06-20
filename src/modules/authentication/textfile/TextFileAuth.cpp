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
#include "system/globalRngGen.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

namespace _Wolframe {
namespace AAAA {

// Text file authentication - authentication unit
//***********************************************************************

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

const char** TextFileAuthUnit::mechs() const
{
	static const char* m[] = { "WOLFRAME-CRAM", NULL };
	return m;
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


bool TextFileAuthUnit::getUser( const std::string& hash, const std::string& key,
				       PwdFileUser& user, bool caseSensitveUser ) const
{
	if ( m_pwdFile.getHMACuser( hash, key, user, caseSensitveUser ))	{
		assert( !user.user.empty() );
		return true;
	}
	else
		user.clear();
	return false;
}

bool TextFileAuthUnit::getUser( const std::string& userHash, PwdFileUser& user,
				       bool caseSensitveUser ) const
{
	if ( m_pwdFile.getHMACuser( userHash, user, caseSensitveUser ))	{
		assert( !user.user.empty() );
		return true;
	}
	else
		user.clear();
	return false;
}

// Text file authentication - authentication slice
//***********************************************************************
TextFileAuthSlice::TextFileAuthSlice( const TextFileAuthUnit& backend )
	: m_backend( backend )
{
	m_challenge = NULL;
	m_state = SLICE_INITIALIZED;
	m_inputReusable = false;
	m_lastSlice = false;
}

TextFileAuthSlice::~TextFileAuthSlice()
{
	m_usr.clear();
	if ( m_challenge != NULL )
		delete m_challenge;
}

void TextFileAuthSlice::dispose()
{
	delete this;
}

/// The input message
void TextFileAuthSlice::messageIn( const std::string& message )
{
	switch ( m_state )	{
		case SLICE_INITIALIZED:	{
			try	{
				m_inputReusable = true;
				if ( m_backend.getUser( message, m_usr ))
					m_state = SLICE_USER_FOUND;
				else	{
					if ( m_lastSlice )
						m_state = SLICE_FAKE_USER;
					else
						m_state = SLICE_USER_NOT_FOUND;
				}
			}
			catch( std::exception& e )	{
				LOG_ERROR << "Text file auth slice: ("
					  << ") exception: " << e.what();
				m_state = SLICE_SYSTEM_FAILURE;
				m_inputReusable = false;
			}
			break;
		}
		case SLICE_USER_FOUND:
			LOG_ERROR << "Text file auth slice: (" << identifier()
				  << ") received message in SLICE_USER_FOUND state";
			m_state = SLICE_SYSTEM_FAILURE;
			m_inputReusable = false;
			break;
		case SLICE_FAKE_USER:
			LOG_ERROR << "Text file auth slice: (" << identifier()
				  << ") received message in SLICE_FAKE_USER state";
			m_state = SLICE_SYSTEM_FAILURE;
			m_inputReusable = false;
			break;
		case SLICE_USER_NOT_FOUND:
			LOG_ERROR << "Text file auth slice: (" << identifier()
				  << ") received message in SLICE_USER_NOT_FOUND state";
			m_inputReusable = false;
			break;
		case SLICE_CHALLENGE_SENT:	{
			m_inputReusable = false;
			PasswordHash hash( m_usr.hash );
			CRAMresponse response( *m_challenge, hash );
			if ( response == message )
				m_state = SLICE_AUTHENTICATED;
			else
				m_state = SLICE_INVALID_CREDENTIALS;
			break;
		}
		case SLICE_INVALID_CREDENTIALS:
			LOG_ERROR << "Text file auth slice: (" << identifier()
				  << ") received message in SLICE_INVALID_CREDENTIALS state";
			m_inputReusable = false;
			break;
		case SLICE_AUTHENTICATED:
			LOG_ERROR << "Text file auth slice: (" << identifier()
				  << ") received message in SLICE_AUTHENTICATED state";
			m_inputReusable = false;
			break;
		case SLICE_SYSTEM_FAILURE:
			LOG_ERROR << "Text file auth slice: (" << identifier()
				  << ") received message in SLICE_SYSTEM_FAILURE state";
			m_inputReusable = false;
			break;
	}
}

/// The output message
std::string TextFileAuthSlice::messageOut()
{
	switch ( m_state )	{
		case SLICE_INITIALIZED:
			LOG_ERROR << "Text file auth slice: (" << identifier()
				  << ") message requested in SLICE_INITIALIZED state";
			m_state = SLICE_SYSTEM_FAILURE;
			break;
		case SLICE_USER_FOUND:	{
			m_challenge = new CRAMchallenge( GlobalRandomGenerator::instance( "" ) );
			m_state = SLICE_CHALLENGE_SENT;
			PasswordHash hash( m_usr.hash );
			return m_challenge->toString( hash.salt() );
		}
		case SLICE_USER_NOT_FOUND:
			LOG_ERROR << "Text file auth slice: (" << identifier()
				  << ") message requested in SLICE_USER_NOT_FOUND state";
			break;
		case SLICE_FAKE_USER:	{
			GlobalRandomGenerator& rnd = GlobalRandomGenerator::instance( "" );
			m_challenge = new CRAMchallenge( rnd );
			m_state = SLICE_CHALLENGE_SENT;
			PasswordHash::Salt salt( rnd );
			return m_challenge->toString( salt );
		}
		case SLICE_CHALLENGE_SENT:
			LOG_ERROR << "Text file auth slice: (" << identifier()
				  << ") message requested in SLICE_CHALLENGE_SENT state";
			m_state = SLICE_SYSTEM_FAILURE;
			break;
		case SLICE_INVALID_CREDENTIALS:
			LOG_ERROR << "Text file auth slice: (" << identifier()
				  << ") message requested in SLICE_INVALID_CREDENTIALS state";
			break;
		case SLICE_AUTHENTICATED:
			LOG_ERROR << "Text file auth slice: (" << identifier()
				  << ") message requested in SLICE_AUTHENTICATED state";
			break;
		case SLICE_SYSTEM_FAILURE:
			LOG_ERROR << "Text file auth slice: (" << identifier()
				  << ") message requested in SLICE_SYSTEM_FAILURE state";
			break;
	}
	return std::string();
}

/// The current status of the authenticator slice
AuthenticatorSlice::Status TextFileAuthSlice::status() const
{
	switch ( m_state )	{
		case SLICE_INITIALIZED:
			return AWAITING_MESSAGE;
		case SLICE_USER_FOUND:
			return MESSAGE_AVAILABLE;
		case SLICE_USER_NOT_FOUND:
			return USER_NOT_FOUND;
		case SLICE_FAKE_USER:
			return USER_NOT_FOUND;
		case SLICE_CHALLENGE_SENT:
			return AWAITING_MESSAGE;
		case SLICE_INVALID_CREDENTIALS:
			return INVALID_CREDENTIALS;
		case SLICE_AUTHENTICATED:
			return AUTHENTICATED;
		case SLICE_SYSTEM_FAILURE:
			return SYSTEM_FAILURE;
	}
	return SYSTEM_FAILURE;		// just to silence compilers
}

/// The authenticated user or NULL if not authenticated
User* TextFileAuthSlice::user()
{
	if ( m_state == SLICE_AUTHENTICATED )	{
		if ( m_usr.user.empty() )
			return NULL;
		User* usr = new User( identifier(), m_usr.user, m_usr.info );
		m_usr.clear();
		return usr;
	}
	else
		return NULL;
}

}} // namespace _Wolframe::AAAA

