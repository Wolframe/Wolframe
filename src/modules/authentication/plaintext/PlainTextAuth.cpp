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
//
//

#include <string>
#include <ostream>
#include <iostream>
#include <fstream>

#include "logger-v1.hpp"
#include "PlainTextAuth.hpp"

#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

namespace _Wolframe {
namespace AAAA {

/// Plain text authentication
//***********************************************************************

PlainTextAuthContainer::PlainTextAuthContainer( const PlainTextAuthConfig& conf )
{
	m_auth = new PlainTextAuthenticator( conf.m_identifier, conf.m_file );
	MOD_LOG_NOTICE << "Plain text authenticator container created";
}


//***********************************************************************

PlainTextAuthenticator::PlainTextAuthenticator( const std::string& Identifier,
						const std::string& filename )
	: AuthenticationUnit( Identifier ), m_file( filename )
{
	MOD_LOG_DEBUG << "Plain text authenticator created with file '" << m_file << "'";
}

PlainTextAuthenticator::~PlainTextAuthenticator()
{
}

AuthenticatorSlice* PlainTextAuthenticator::authSlice()
{
	return new PlainTextAuthSlice( *this );
}

User* PlainTextAuthenticator::authenticate( std::string username, std::string password,
					    bool caseSensitveUser ) const
{
	try	{
		std::ifstream	pwdFile( m_file.c_str() );
		MOD_LOG_TRACE << "Plain text authenticator opened file '" << m_file << "'";
		while( !pwdFile.eof())	{
			std::string line;
			std::getline( pwdFile, line );
			boost::algorithm::trim( line );
			if ( line[0] == '#' || line.empty() )
				continue;

			std::size_t  start = 0, end = 0;
			std::string usr, pwd, name;
			if ( end != std::string::npos)	{
				end = line.find( ":", start );
				usr = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
				start = (( end > ( std::string::npos - 1 )) ?  std::string::npos : end + 1 );
			}
//MOD_LOG_TRACE << "Plain text authenticator user: '" << usr << "'";
			if (( caseSensitveUser && usr != username ) ||
					( !caseSensitveUser && !boost::algorithm::iequals( usr, username )))
				continue;

			if ( end != std::string::npos)	{
				end = line.find( ":", start );
				pwd = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
				start = (( end > ( std::string::npos - 1 )) ?  std::string::npos : end + 1 );
			}
//MOD_LOG_TRACE << "Plain text authenticator password: '" << pwd << "'";
			if ( pwd != password )
				return NULL;

			if ( end != std::string::npos)	{
				end = line.find( ":", start );
				name = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
				start = (( end > ( std::string::npos - 1 )) ?  std::string::npos : end + 1 );
			}
//MOD_LOG_TRACE << "Plain text authenticator name: '" << name << "'";
			return new User( "PlainText", usr, name );
		}
		return NULL;
	}
	catch( std::exception& e)	{
		MOD_LOG_TRACE << "Unable to read plain text authentication file '" << m_file << "': " << e.what();
		return NULL;
	}
}


//***********************************************************************
PlainTextAuthSlice::PlainTextAuthSlice( const PlainTextAuthenticator& backend )
	: m_backend( backend )
{
	m_user = NULL;
}

PlainTextAuthSlice::~PlainTextAuthSlice()
{
	if ( m_user != NULL )
		delete m_user;
}

void PlainTextAuthSlice::receiveData( const void* /*data*/, std::size_t /*size*/ )
{
}

const FSMoperation PlainTextAuthSlice::nextOperation()
{
	FSMoperation	op;
	return op;
}

void PlainTextAuthSlice::signal( FSMsignal /*event*/ )
{
}

std::size_t PlainTextAuthSlice::dataLeft( const void*& /*begin*/ )
{
	return 0;
}

User* PlainTextAuthSlice::user()
{
	User* ret = m_user;
	m_user = NULL;
	return ret;
}

}} // namespace _Wolframe::AAAA

