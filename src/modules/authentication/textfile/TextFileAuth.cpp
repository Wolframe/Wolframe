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
#include "AAAA/CRAM.hpp"
#include "AAAA/passwordHash.hpp"
#include "AAAA/pwdChangeMessage.hpp"
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


AuthenticatorSlice* TextFileAuthUnit::slice( const std::string& mech,
					     const net::RemoteEndpoint& /*client*/ )
{
	if ( boost::to_upper_copy( mech ) == "WOLFRAME-CRAM" )
		return new TextFileAuthSlice( *this );
	else
		return NULL;
}


PasswordChanger* TextFileAuthUnit::passwordChanger( const User& user,
						    const net::RemoteEndpoint& /*client*/ )
{
	if ( boost::to_upper_copy( user.mech() ) == "WOLFRAME-CRAM" &&
			user.authenticator() == identifier() )
		return new TextFilePwdChanger( *this, user.uname() );
	else
		return NULL;
}

User* TextFileAuthUnit::authenticatePlain( const std::string& username,
					   const std::string& password ) const
{
	PwdFileUser	user;

	if ( m_pwdFile.getUser( username, user ))	{
		PasswordHash	filePwd( user.hash );
		PasswordHash	clientPwd( filePwd.salt(), password );

		PasswordHash::Hash	h0 = filePwd.hash();
		PasswordHash::Hash	h1 = clientPwd.hash();
		if ( h0 == h1 )
			return new User( identifier(), "PLAIN", user.user, user.info );
	}
	return NULL;
}


bool TextFileAuthUnit::getUser( const std::string& hash, const std::string& key,
				PwdFileUser& user ) const
{
	if ( m_pwdFile.getHMACuser( hash, key, user ))	{
		assert( !user.user.empty() );
		return true;
	}
	else
		user.clear();
	return false;
}

bool TextFileAuthUnit::getUser( const std::string& userHash, PwdFileUser& user ) const
{
	if ( m_pwdFile.getHMACuser( userHash, user ))	{
		assert( !user.user.empty() );
		return true;
	}
	else
		user.clear();
	return false;
}

bool TextFileAuthUnit::getUserPlain( const std::string& username, PwdFileUser& user ) const
{
	if ( m_pwdFile.getUser( username, user ))	{
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
	m_inputReusable = true;
	m_lastSlice = false;
	m_fakeUser = false;
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
					if ( m_lastSlice )	{
						m_fakeUser = true;
						m_state = SLICE_USER_FOUND;
					}
					else
						m_state = SLICE_USER_NOT_FOUND;
				}
			}
			catch( std::exception& e )	{
				LOG_ERROR << "Text file auth slice (" << identifier()
					  << ") exception: " << e.what();
				m_state = SLICE_SYSTEM_FAILURE;
				m_inputReusable = false;
			}
			break;
		}
		case SLICE_USER_FOUND:	{
			std::string msg = "Text file auth slice (" + identifier() +
					  "): received message in SLICE_USER_FOUND state";
			LOG_ALERT << msg;
			m_state = SLICE_SYSTEM_FAILURE;
			m_inputReusable = false;
			throw std::logic_error( msg );
			break;
		}
		case SLICE_USER_NOT_FOUND:	{
			std::string msg = "Text file auth slice (" + identifier() +
					  "): received message in SLICE_USER_NOT_FOUND state";
			LOG_ALERT << msg;
			m_state = SLICE_SYSTEM_FAILURE;
			m_inputReusable = false;
			throw std::logic_error( msg );
			break;
		}
		case SLICE_CHALLENGE_SENT:	{
			m_inputReusable = false;
			if ( !m_fakeUser )	{
				PasswordHash hash( m_usr.hash );
				CRAMresponse response( *m_challenge, hash );
				if ( response == message )
					m_state = SLICE_AUTHENTICATED;
				else
					m_state = SLICE_INVALID_CREDENTIALS;
			}
			else
				m_state = SLICE_INVALID_CREDENTIALS;
			break;
		}
		case SLICE_INVALID_CREDENTIALS:	{
			std::string msg = "Text file auth slice (" + identifier() +
					  "): received message in SLICE_INVALID_CREDENTIALS state";
			LOG_ALERT << msg;
			m_state = SLICE_SYSTEM_FAILURE;
			m_inputReusable = false;
			throw std::logic_error( msg );
			break;
		}
		case SLICE_AUTHENTICATED:	{
			std::string msg = "Text file auth slice (" + identifier() +
					  "): received message in SLICE_AUTHENTICATED state";
			LOG_ALERT << msg;
			m_state = SLICE_SYSTEM_FAILURE;
			m_inputReusable = false;
			throw std::logic_error( msg );
			break;
		}
		case SLICE_SYSTEM_FAILURE:	{
			std::string msg = "Text file auth slice (" + identifier() +
					  "): received message in SLICE_SYSTEM_FAILURE state";
			LOG_ALERT << msg;
			m_inputReusable = false;
			throw std::logic_error( msg );
			break;
		}
	}
}

/// The output message
std::string TextFileAuthSlice::messageOut()
{
	switch ( m_state )	{
		case SLICE_INITIALIZED:	{
			std::string msg = "Text file auth slice (" + identifier() +
					  "): message requested in SLICE_INITIALIZED state";
			LOG_ALERT << msg;
			m_state = SLICE_SYSTEM_FAILURE;
			m_inputReusable = false;
			throw std::logic_error( msg );
			break;
		}
		case SLICE_USER_FOUND:	{
			GlobalRandomGenerator& rnd = GlobalRandomGenerator::instance( "" );
			m_challenge = new CRAMchallenge( rnd );
			std::string challenge;
			if ( m_fakeUser )	{
				PasswordHash::Salt salt( rnd );
				challenge = m_challenge->toString( salt );
			}
			else	{
				PasswordHash hash( m_usr.hash );
				challenge = m_challenge->toString( hash.salt() );
			}
			m_state = SLICE_CHALLENGE_SENT;
			m_inputReusable = false;
			return challenge;
		}
		case SLICE_USER_NOT_FOUND:	{
			std::string msg = "Text file auth slice (" + identifier() +
					  "): message requested in SLICE_USER_NOT_FOUND state";
			LOG_ALERT << msg;
			m_state = SLICE_SYSTEM_FAILURE;
			m_inputReusable = false;
			throw std::logic_error( msg );
			break;
		}
		case SLICE_CHALLENGE_SENT:	{
			std::string msg = "Text file auth slice (" + identifier() +
					  "): message requested in SLICE_CHALLENGE_SENT state";
			LOG_ALERT << msg;
			m_state = SLICE_SYSTEM_FAILURE;
			m_inputReusable = false;
			throw std::logic_error( msg );
			break;
		}
		case SLICE_INVALID_CREDENTIALS:	{
			std::string msg = "Text file auth slice (" + identifier() +
					  "): message requested in SLICE_INVALID_CREDENTIALS state";
			LOG_ALERT << msg;
			m_state = SLICE_SYSTEM_FAILURE;
			m_inputReusable = false;
			throw std::logic_error( msg );
			break;
		}
		case SLICE_AUTHENTICATED:	{
			std::string msg = "Text file auth slice (" + identifier() +
					  "): message requested in SLICE_AUTHENTICATED state";
			LOG_ALERT << msg;
			m_state = SLICE_SYSTEM_FAILURE;
			m_inputReusable = false;
			throw std::logic_error( msg );
			break;
		}
		case SLICE_SYSTEM_FAILURE:	{
			std::string msg = "Text file auth slice (" + identifier() +
					  "): message requested in SLICE_SYSTEM_FAILURE state";
			LOG_ALERT << msg;
			m_inputReusable = false;
			throw std::logic_error( msg );
			break;
		}
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
		User* usr = new User( identifier(), "WOLFRAME-CRAM", m_usr.user, m_usr.info );
		m_usr.clear();
		return usr;
	}
	else
		return NULL;
}


// Text file authentication - password changer
//***********************************************************************
TextFilePwdChanger::TextFilePwdChanger( const TextFileAuthUnit& backend,
					const std::string& username )
	: m_backend( backend )
{
	m_challenge = NULL;
	if ( m_backend.getUserPlain( username, m_usr ))
		m_state = CHANGER_INITIALIZED;
	else
		m_state = CHANGER_SYSTEM_FAILURE;
}

TextFilePwdChanger::~TextFilePwdChanger()
{
	m_usr.clear();
	for ( std::size_t i = 0; i < m_password.length(); i++ )
		m_password[ i ] = 'x';
	m_password.clear();
	if ( m_challenge != NULL )
		delete m_challenge;
}

void TextFilePwdChanger::dispose()
{
	delete this;
}

/// The input message
void TextFilePwdChanger::messageIn( const std::string& message )
{
	switch ( m_state )	{
		case CHANGER_INITIALIZED:	{
			std::string msg = "Text file password changer (" + identifier() +
					  "): received message in CHANGER_INITIALIZED state";
			LOG_ALERT << msg;
			m_state = CHANGER_SYSTEM_FAILURE;
			throw std::logic_error( msg );
			break;
		}
		case CHANGER_CHALLENGE_SENT:	{
			PasswordHash hash( m_usr.hash );
			CRAMresponse response( *m_challenge, hash );
			PasswordChangeMessage pwd;
			if ( pwd.fromBase64( message, hash.salt().salt(), response.response() ))	{
				m_password = pwd.password();
				pwd.clear();
				m_state = CHANGER_PASSWORD_EXCHANGED;
			}
			else
				m_state = CHANGER_INVALID_MESSAGE;
			break;
		}
		case CHANGER_INVALID_MESSAGE:	{
			std::string msg = "Text file password changer (" + identifier() +
					  "): received message in CHANGER_INVALID_DATA state";
			LOG_ALERT << msg;
			m_state = CHANGER_SYSTEM_FAILURE;
			throw std::logic_error( msg );
			break;
		}
		case CHANGER_PASSWORD_EXCHANGED:	{
			std::string msg = "Text file password changer (" + identifier() +
					  "): received message in CHANGER_PASSWORD_EXCHANGED state";
			LOG_ALERT << msg;
			m_state = CHANGER_SYSTEM_FAILURE;
			throw std::logic_error( msg );
			break;
		}
		case CHANGER_SYSTEM_FAILURE:	{
			std::string msg = "Text file password changer (" + identifier() +
					  "): received message in CHANGER_SYSTEM_FAILURE state";
			LOG_ALERT << msg;
			m_state = CHANGER_SYSTEM_FAILURE;
			throw std::logic_error( msg );
			break;
		}
	}
}

/// The output message
std::string TextFilePwdChanger::messageOut()
{
	switch ( m_state )	{
		case CHANGER_INITIALIZED:	{
			GlobalRandomGenerator& rnd = GlobalRandomGenerator::instance( "" );
			m_challenge = new CRAMchallenge( rnd );
			PasswordHash hash( m_usr.hash );
			std::string challenge = m_challenge->toString( hash.salt() );
			m_state = CHANGER_CHALLENGE_SENT;
			return challenge;
		}
		case CHANGER_CHALLENGE_SENT:	{
			std::string msg = "Text file password changer (" + identifier() +
					  "): message requested in CHANGER_CHALLENGE_SENT state";
			LOG_ALERT << msg;
			throw std::logic_error( msg );
			break;
		}
		case CHANGER_INVALID_MESSAGE:	{
			std::string msg = "Text file password changer (" + identifier() +
					  "): message requested in CHANGER_INVALID_DATA state";
			LOG_ALERT << msg;
			throw std::logic_error( msg );
			break;
		}
		case CHANGER_PASSWORD_EXCHANGED:	{
			std::string msg = "Text file password changer (" + identifier() +
					  "): message requested in CHANGER_PASSWORD_EXCHANGED state";
			LOG_ALERT << msg;
			throw std::logic_error( msg );
			break;
		}
		case CHANGER_SYSTEM_FAILURE:	{
			std::string msg = "Text file password changer (" + identifier() +
					  "): message requested in CHANGER_SYSTEM_FAILURE state";
			LOG_ALERT << msg;
			throw std::logic_error( msg );
			break;
		}
	}
	return std::string();
}

/// The current status of the password changer
PasswordChanger::Status TextFilePwdChanger::status() const
{
	switch ( m_state )	{
		case CHANGER_INITIALIZED:
			return PasswordChanger::MESSAGE_AVAILABLE;
		case CHANGER_CHALLENGE_SENT:
			return PasswordChanger::AWAITING_MESSAGE;
		case CHANGER_INVALID_MESSAGE:
			return PasswordChanger::INVALID_MESSAGE;
		case CHANGER_PASSWORD_EXCHANGED:
			return PasswordChanger::PASSWORD_EXCHANGED;
		case CHANGER_SYSTEM_FAILURE:
			return PasswordChanger::SYSTEM_FAILURE;
	}
	return SYSTEM_FAILURE;		// just to silence some compilers
}

/// The new password
std::string TextFilePwdChanger::password()
{
	if ( m_state == CHANGER_PASSWORD_EXCHANGED )
		return m_password;
	else	{
		std::string msg = "Password changer (" + identifier() + ") : password requested in " +
				  statusName( m_state ) + " state";
		LOG_ALERT << msg;
		throw std::logic_error( msg );
	}
}

}} // namespace _Wolframe::AAAA

