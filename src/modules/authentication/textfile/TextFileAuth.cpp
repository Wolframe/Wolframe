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
// Text file authentication
//

#include <string>

#include <cstdio>
#include <cerrno>

#include "logger-v1.hpp"
#include "TextFileAuth.hpp"
#include "sha2.h"
#include "byte2hex.h"
#include "AAAA/CRAM.hpp"

#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

namespace _Wolframe {
namespace AAAA {

// Text file authentication - container
//***********************************************************************
TextFileAuthContainer::TextFileAuthContainer( const TextFileAuthConfig& conf )
{
	m_auth = new TextFileAuthenticator( conf.m_identifier, conf.m_file );
	MOD_LOG_DEBUG << "Text file authenticator container created for '"
		      << conf.m_identifier << "'";
}


// Text file authentication - authentication unit
//***********************************************************************
TextFileAuthenticator::TextFileAuthenticator( const std::string& Identifier,
					      const std::string& filename )
	: AuthenticationUnit( Identifier ), m_file( filename )
{
	MOD_LOG_DEBUG << "Text file authenticator '" << identifier()
		      << "' created with file '" << m_file << "'";
}

TextFileAuthenticator::~TextFileAuthenticator()
{
}

AuthenticatorSlice* TextFileAuthenticator::authSlice()
{
	return new TextFileAuthSlice( *this );
}


static const std::size_t PWD_LINE_SIZE = 1024;

User* TextFileAuthenticator::authenticatePlain( const std::string& username,
						const std::string& password,
						bool caseSensitveUser ) const
{
	FILE*	pwdFile;
	if ( ! ( pwdFile = fopen( m_file.c_str(), "r" )))	{
		MOD_LOG_ERROR << "Authentication: " << identifier() << ": unable to open authentication file '"
			      << m_file << "': " << strerror( errno );
		return NULL;
	}
	MOD_LOG_TRACE << "Authentication: " << identifier() << ": opened file '" << m_file << "'";

	char	lineBuf[ PWD_LINE_SIZE + 2];
	while( ! feof( pwdFile ))	{
		if ( ! fgets( lineBuf,  PWD_LINE_SIZE, pwdFile ))	{
			int err = errno;
			if ( feof( pwdFile ))	{		// EOF reached with nothing to read
				fclose( pwdFile );
				return NULL;
			}
			else	{
				MOD_LOG_ERROR << "Authentication: " << identifier()
					      << ": unable to read from authentication file '"
					      << m_file << "': " << strerror( err );
				fclose( pwdFile );
				return NULL;
			}
		}

		std::string line( lineBuf );
		boost::algorithm::trim( line );
		if ( line[0] == '#' || line.empty() )
			continue;

		std::size_t  start = 0, end = 0;
		std::string usr, pwd, name;
		if ( end != std::string::npos )	{
			end = line.find( ":", start );
			usr = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
			start = (( end > ( std::string::npos - 1 )) ?  std::string::npos : end + 1 );
		}
		if (( caseSensitveUser && usr != username ) ||
				( !caseSensitveUser && !boost::algorithm::iequals( usr, username )))
			continue;

		if ( end != std::string::npos )	{
			end = line.find( ":", start );
			pwd = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
			start = (( end > ( std::string::npos - 1 )) ?  std::string::npos : end + 1 );
		}

		unsigned char pwDigest[ SHA224_DIGEST_SIZE ];
		if ( hex2byte( pwd.c_str(), pwDigest, SHA224_DIGEST_SIZE ) < 0 )	{
			MOD_LOG_ERROR << "Authentication: " << identifier() << ": error parsing password hash for user '"
				      << usr << "'";
			fclose( pwdFile );
			return NULL;
		}
		unsigned char inDigest[ SHA224_DIGEST_SIZE ];
		sha224((const unsigned char *)password.c_str(), password.length(), inDigest );

		if ( memcmp( pwDigest, inDigest, SHA224_DIGEST_SIZE ))	{
			fclose( pwdFile );
			return NULL;
		}

		if ( end != std::string::npos )	{
			end = line.find( ":", start );
			name = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
			boost::algorithm::trim( name );
		}

		if ( name.empty())
			MOD_LOG_WARNING << "Authentication: " << identifier() << ": no name for user '"
					<< usr << "'";
		fclose( pwdFile );
		return new User( "TextFile", usr, name );
	}
	fclose( pwdFile );
	return NULL;
}


User* TextFileAuthenticator::authenticate( const CRAMchallenge& challenge,
					   const CRAMresponse& response,
					   bool caseSensitveUser ) const
{
	return authenticate( challenge, response.toString(), caseSensitveUser );
}

User* TextFileAuthenticator::authenticate( const CRAMchallenge& challenge,
					   const std::string& response,
					   bool caseSensitveUser ) const
{
	FILE*	pwdFile;
	if ( ! ( pwdFile = fopen( m_file.c_str(), "r" )))	{
		MOD_LOG_ERROR << "Authentication: " << identifier() << ": unable to open authentication file '"
			      << m_file << "': " << strerror( errno );
		return NULL;
	}
	MOD_LOG_TRACE << "Authentication: " << identifier() << ": opened file '" << m_file << "'";

	char	lineBuf[ PWD_LINE_SIZE + 2];
	while( ! feof( pwdFile ))	{
		if ( ! fgets( lineBuf,  PWD_LINE_SIZE, pwdFile ))	{
			int err = errno;
			if ( feof( pwdFile ))	{		// EOF reached with nothing to read
				fclose( pwdFile );
				return NULL;
			}
			else	{
				MOD_LOG_ERROR << "Authentication: " << identifier()
					      << ": unable to read from authentication file '"
					      << m_file << "': " << strerror( err );
				fclose( pwdFile );
				return NULL;
			}
		}

		std::string line( lineBuf );
		boost::algorithm::trim( line );
		if ( line[0] == '#' || line.empty() )
			continue;

		std::size_t  start = 0, end = 0;
		std::string usr, pwd, name;
		if ( end != std::string::npos )	{
			end = line.find( ":", start );
			usr = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
			start = (( end > ( std::string::npos - 1 )) ?  std::string::npos : end + 1 );
		}
		if ( caseSensitveUser  )
			boost::to_lower( usr );

		if ( end != std::string::npos )	{
			end = line.find( ":", start );
			pwd = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
			start = (( end > ( std::string::npos - 1 )) ?  std::string::npos : end + 1 );
		}

		try	{
			CRAMresponse	local( challenge, usr, pwd );
			if ( local != response )
				continue;
		}
		catch ( std::exception& e)	{
			MOD_LOG_ERROR << "Authentication: " << identifier() << ": error parsing password hash for user '"
				      << usr << "'";
			continue;
		}

		if ( end != std::string::npos )	{
			end = line.find( ":", start );
			name = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
			boost::algorithm::trim( name );
		}

		if ( name.empty())
			MOD_LOG_WARNING << "Authentication: " << identifier() << ": no name for user '"
					<< usr << "'";
		fclose( pwdFile );
		return new User( "TextFile", usr, name );
	}
	fclose( pwdFile );
	return NULL;
}


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

const FSMoperation TextFileAuthSlice::nextOperation()
{
	FSMoperation	op;
	return op;
}

void TextFileAuthSlice::signal( FSMsignal /*event*/ )
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

