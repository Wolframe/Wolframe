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
//
//

#include <string>

#include "passwdFile.hpp"

#include <sstream>
#include <iostream>

#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "utils/miscUtils.hpp"

namespace _Wolframe {
namespace AAAA {

static const std::size_t PWD_LINE_SIZE = 1024;

std::string PasswordFile::passwdLine( const std::string& user,
				      const std::string& password , const std::string &info )
{
	std::stringstream ss;
	ss << "Display only, user '" << user << "' with password '" << password << "', info: '"
	   << info << "'\n";
	return ss.str();
}

bool PasswordFile::parsePwdLine( const std::string& pwdLine,
				 std::string& user, std::string& password, std::string& info )
{
	std::string line = boost::algorithm::trim_copy( pwdLine );
	if ( line.empty() || line[0] == '#' )	{
		user.clear();
		password.clear();
		info.clear();
		return true;
	}

	std::size_t  start = 0, end = 0;
	if ( end != std::string::npos )	{
		end = line.find( ":", start );
		user = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
		start = (( end > ( std::string::npos - 1 )) ?  std::string::npos : end + 1 );
		boost::algorithm::trim( user );
	}

	if ( end != std::string::npos )	{
		end = line.find( ":", start );
		password = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
		start = (( end > ( std::string::npos - 1 )) ?  std::string::npos : end + 1 );
		boost::algorithm::trim( password );
	}
	else	{
		password.clear();
		info.clear();
		return false;
	}

	if ( end != std::string::npos )	{
		end = line.find( ":", start );
		info = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
		boost::algorithm::trim( info );
	}
	return true;
}

std::string PasswordFile::salt()
{
	if ( !boost::filesystem::exists( m_filename ))	{
		std::string msg = "password file '";
		msg += m_filename + "' does not exist";
		throw std::runtime_error( msg );
	}

	FILE*	file;
	if ( ! ( file = fopen( m_filename.c_str(), "r" )))	{
		std::string msg = "error opening password file '";
		msg += m_filename + "'";
		throw std::runtime_error( msg );
	}

	char lineBuf[ PWD_LINE_SIZE + 2];
	while( ! feof( file ))	{
		if ( ! fgets( lineBuf,  PWD_LINE_SIZE, file ))	{
			if ( feof( file ))	{		// EOF reached with nothing to read
				fclose( file );
				return std::string();
			}
			else	{
				std::string msg = "error reading password file '";
				msg += m_filename + "'";
				fclose( file );
				throw std::runtime_error( msg );
			}
		}

		std::string line( lineBuf );
		boost::algorithm::trim( line );
		if ( line[0] == '#' || line.empty() )
			continue;

		std::size_t  start = 0, end = 0;
		std::string pwd;
		// skip username
		if ( end != std::string::npos )	{
			end = line.find( ":", start );
			line.substr( start, (end == std::string::npos) ? std::string::npos : end - start );
			start = (( end > ( std::string::npos - 1 )) ?  std::string::npos : end + 1 );
		}
		// get password
		if ( end != std::string::npos )	{
			end = line.find( ":", start );
			pwd = ( line.substr( start, (end == std::string::npos) ? std::string::npos : end - start ));
			start = (( end > ( std::string::npos - 1 )) ?  std::string::npos : end + 1 );
		}
		return pwd;
	}

	return std::string( "" );
}

void PasswordFile::addUser(const std::string& /*user*/, const std::string& /*password*/ )
{
//	FILE*	file;

	if ( !boost::filesystem::exists( m_filename ))	{
		if ( m_create )	{
		}
		else	{
			std::string msg = "password file '";
			msg += m_filename + "' does not exist";
			throw std::runtime_error( msg );
		}
	}
	else	{

	}
}

bool PasswordFile::delUser( const std::string& user )
{
	std::cout << "Delete user '" << user << "' from password file '"
		  << m_filename << "'\n";
	return true;
}

bool getUser( const std::string& /*user*/, std::string& /*password*/ )
{
	return true;
}

bool getUser( const std::string& /*challenge*/, const std::string& /*response*/,
	      std::string& /*user*/, std::string& /*password*/ )
{
	return true;
}


}} // namepspace _Wolframe::AAAA
