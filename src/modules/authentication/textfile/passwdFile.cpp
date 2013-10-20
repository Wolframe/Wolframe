/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
#include <cstdio>
#include <cassert>
#include <sstream>
#include <iostream>

#include "types/base64.h"
#include "passwdFile.hpp"
#include "crypto/HMAC.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

namespace _Wolframe {
namespace AAAA {

static const std::size_t PWD_FILE_LINE_SIZE = 1024 + 1;
static const std::size_t MAX_HMAC_KEY_SIZE = 1024;

std::string PasswordFile::passwdLine( const PwdFileUser& user )
{
	std::stringstream ss;
	ss << user.user << ":" << user.hash << ":" << user.info << ":" << user.expiry;
	return ss.str();
}

static std::string pwdLineUser( const char* pwdLine )
{
	std::string line = pwdLine;
	boost::algorithm::trim( line );

	if ( line.empty() || line[0] == '#' )
		return "";

	std::size_t end = line.find( ":", 0 );
	std::string user = ( line.substr( 0, (end == std::string::npos) ? std::string::npos : end ));
	boost::algorithm::trim( user );
	return user;
}

static bool parsePwdLine( const char* pwdLine, PwdFileUser& user )
{
	std::string line = pwdLine;
	boost::algorithm::trim( line );
	if ( line.empty() || line[0] == '#' )
		return false;

	std::size_t  start = 0, end = 0;
	if ( end != std::string::npos )	{
		end = line.find( ":", start );
		user.user = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
		start = (( end > ( std::string::npos - 1 )) ?  std::string::npos : end + 1 );
		boost::algorithm::trim( user.user );
	}

	if ( end != std::string::npos )	{
		end = line.find( ":", start );
		user.hash = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
		start = (( end > ( std::string::npos - 1 )) ?  std::string::npos : end + 1 );
		boost::algorithm::trim( user.hash );
	}

	if ( end != std::string::npos )	{
		end = line.find( ":", start );
		user.info = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
		start = (( end > ( std::string::npos - 1 )) ?  std::string::npos : end + 1 );
		boost::algorithm::trim( user.info );
	}

	if ( end != std::string::npos )	{
		end = line.find( ":", start );
		std::string expiry = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
		user.expiry = boost::lexical_cast<unsigned long>( expiry );
	}

	return true;
}


bool PasswordFile::addUser( const PwdFileUser& user, bool caseSensitive )
{
	FILE*	file;

	if ( !boost::filesystem::exists( m_filename ) )	{
		if ( !m_create )	{
			std::string msg = "password file '";
			msg += m_filename + "' does not exist";
			throw std::runtime_error( msg );
		}
		else	{
			if (( file = fopen( m_filename.c_str(), "w+" )) == NULL )	{
				int err = errno;
				std::string msg = "password file '";
				msg += m_filename + "' could not be opened: " + strerror( err );
				throw std::runtime_error( msg );
			}
		}
	}
	else	{
		PwdFileUser tmpUser;
		if ( getUser( user.user, tmpUser, caseSensitive ))
			return false;
		if (( file = fopen( m_filename.c_str(), "a+" )) == NULL )	{
			int err = errno;
			std::string msg = "password file '";
			msg += m_filename + "' could not be opened: " + strerror( err );
			throw std::runtime_error( msg );
		}
	}

	std::string pwdLine = passwdLine( user );
	assert( !pwdLine.empty() );			// the password line should not be empty
	assert( *pwdLine.rbegin() != '\n' );		// there should be no newline yet
	pwdLine += "\n";
	if ( fputs( pwdLine.c_str(), file ) < 0 )	{
		int err = errno;
		fclose( file );
		std::string msg = "error writing to password file '";
		msg += m_filename + "': " + strerror( err );
		throw std::runtime_error( msg );
	}
	fclose( file );
	return true;
}

bool PasswordFile::delUser( const std::string& username, bool caseSensitive )
{
	PwdFileUser user;
	if ( ! getUser( username, user, caseSensitive ))
		return false;
	std::cout << "Delete user '" << username << "' from password file '"
		  << m_filename << "'\n";
	return true;
}

bool PasswordFile::getUser( const std::string& username, PwdFileUser& user,
			    bool caseSensitive ) const
{
	FILE*	file;
	char	line[ PWD_FILE_LINE_SIZE ];

	if ( !boost::filesystem::exists( m_filename ) && !m_create )	{
		std::string msg = "password file '";
		msg += m_filename + "' does not exist";
		throw std::runtime_error( msg );
	}

	if (( file = fopen( m_filename.c_str(), "r" )) == NULL )	{
		int err = errno;
		std::string msg = "password file '";
		msg += m_filename + "' could not be opened: " + strerror( err );
		throw std::runtime_error( msg );
	}

	std::string trimmedName = boost::algorithm::trim_copy( username );
	if ( !caseSensitive )
		boost::algorithm::to_lower( trimmedName );
	while ( !feof( file ) )	{
		char* ret = fgets( line, PWD_FILE_LINE_SIZE, file );
		if ( ret == NULL )	{
			if ( feof( file ))	{
				fclose( file );
				return false;
			}
			else	{
				int err = errno;
				fclose( file );
				std::string msg = "error reading from password file '";
				msg += m_filename + "': " + strerror( err );
				throw std::runtime_error( msg );
			}
		}
		std::string uname = pwdLineUser( line );
		if ( uname.empty() )
			continue;
		if ( !caseSensitive )
			boost::algorithm::to_lower( uname );
		if ( uname == trimmedName )	{
			fclose( file );
			bool found = parsePwdLine( line, user );
			assert( found == true );
			return true;
		}
	}
	return false;
}

bool PasswordFile::getHMACuser( const std::string& hash, const std::string& key,
				PwdFileUser& user, bool caseSensitive ) const
{
	FILE*		file;
	char		line[ PWD_FILE_LINE_SIZE ];
	unsigned char	binKey[ MAX_HMAC_KEY_SIZE ];
	int		keySize;

	HMAC_SHA256 userHash( hash );
	if (( keySize = base64_decode( key.data(), key.size(),
				       binKey, MAX_HMAC_KEY_SIZE )) < 0 )	{
		std::string msg = "Cannot convert '" + key + "' to a HMAC binary key";
		throw std::runtime_error( msg );
	}

	if ( !boost::filesystem::exists( m_filename ) && !m_create )	{
		std::string msg = "password file '";
		msg += m_filename + "' does not exist";
		throw std::runtime_error( msg );
	}

	if (( file = fopen( m_filename.c_str(), "r" )) == NULL )	{
		int err = errno;
		std::string msg = "password file '";
		msg += m_filename + "' could not be opened: " + strerror( err );
		throw std::runtime_error( msg );
	}

	while ( !feof( file ) )	{
		char* ret = fgets( line, PWD_FILE_LINE_SIZE, file );
		if ( ret == NULL )	{
			if ( feof( file ))	{
				fclose( file );
				return false;
			}
			else	{
				int err = errno;
				fclose( file );
				std::string msg = "error reading from password file '";
				msg += m_filename + "': " + strerror( err );
				throw std::runtime_error( msg );
			}
		}
		std::string uname = pwdLineUser( line );
		if ( uname.empty())
			continue;
		if ( !caseSensitive )
			boost::algorithm::to_lower( uname );
		HMAC_SHA256	hsh( binKey, keySize, uname );
		if ( hsh == userHash )	{
			fclose( file );
			bool found = parsePwdLine( line, user );
			assert( found == true );
			return true;
		}
	}
	return false;
}

}} // namepspace _Wolframe::AAAA
