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

#include "passwdFile.hpp"

#include <sstream>
#include <iostream>

namespace _Wolframe {
namespace AAAA {

std::string PasswordFile::passwdString( const std::string& user,
					const std::string& password )
{
	std::stringstream ss;
	ss << "Display only, user '" << user << "' with password '" << password << "'\n";
	return ss.str();
}

void PasswordFile::addUser(const std::string& user, const std::string& password )
{
	if ( m_create )
		std::cout << "Create password file '" << m_filename
			  << "' if it doesn't exist\n";
	std::cout << "Change / add user '" << user << "', password file '\n"
		  << m_filename << "', password '" << password << "'";
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
