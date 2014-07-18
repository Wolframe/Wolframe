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
///
///\file user.hpp
/// Basic user information

#ifndef _USER_HPP_INCLUDED
#define _USER_HPP_INCLUDED
#include "types/secureString.hpp"
#include <string>
#include <ctime>

namespace _Wolframe {
namespace AAAA {

/// \class User
/// \brief Data structure containing all login data of a user
class User
{
public:
	User()
		: m_loginTime( time( NULL ))			{}
	User( const User& o )
		: m_authenticator( o.m_authenticator ), m_mech( o.m_mech ),
		  m_loginTime( o.m_loginTime ),
		  m_uname( o.m_uname ), m_name( o.m_name )	{}
	User( const std::string& Authenticator, const std::string& Mech,
	      const std::string& uName, const std::string& Name )
		: m_authenticator( Authenticator ), m_mech( Mech ),
		  m_loginTime( time( NULL )),
		  m_uname( uName ), m_name( Name )		{}

	/// Destructor
	~User()						{m_loginTime=0;}

	/// Return the authenticator identifier
	const std::string& authenticator() const	{ return m_authenticator; }
	/// Return the authentication mech
	const std::string& mech() const			{ return m_mech; }
	/// Return the login moment
	time_t loginTime() const			{ return m_loginTime; }
	/// Return the username
	const std::string& uname() const		{ return m_uname; }
	/// Return the real name of the user
	const std::string& name() const			{ return m_name; }

private:
	const types::SecureString	m_authenticator;	///< authenticator identifier
	const types::SecureString	m_mech;			///< authentication mech
	time_t				m_loginTime;		///< login time
	const types::SecureString	m_uname;		///< username
	const types::SecureString	m_name;			///< name of the user
};

}} // namespace _Wolframe::AAAA

#endif // _USER_HPP_INCLUDED
