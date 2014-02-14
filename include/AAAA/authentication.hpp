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
/// \file authentication.hpp
/// \brief top-level header file for authentication interface
///

#ifndef _AUTHENTICATION_HPP_INCLUDED
#define _AUTHENTICATION_HPP_INCLUDED

#include <string>

#include "user.hpp"
#include "database/DBprovider.hpp"
#include "system/FSMinterface.hpp"

namespace _Wolframe {
namespace AAAA {

// interface for the authentication global mechanism
class Authenticator : public _Wolframe::FSM
{
public:
	virtual ~Authenticator()		{}

	// close the authenticator and destroy all sensible data
	virtual void close()			{}
};


/// AuthenticatorInstance
/// This is the base class for authenticator slices implementations
/// An authenticator has (usually) several authenticator instances
class AuthenticatorInstance : public _Wolframe::FSM
{
public:
	enum AuthProtocol	{
		PLAIN,			/// Plain text
		CRAM,			/// Challenge-response
		SASL			/// SASL dialog
	};

	virtual ~AuthenticatorInstance()		{}
	virtual void close()			{}

	virtual AuthProtocol protocolType() const = 0;
	virtual User* user() = 0;
};

/// AuthenticationUnit Unit
/// This is the base class for authentication unit implementations
class AuthenticationUnit
{
public:
	AuthenticationUnit( const std::string& id )
		: m_identifier( id )		{}

	virtual ~AuthenticationUnit()		{}

	const std::string& identifier() const	{ return m_identifier; }

	virtual bool resolveDB( const db::DatabaseProvider& /*db*/ )
						{ return true; }
	virtual const char* className() const = 0;

	virtual AuthenticatorInstance* instance() = 0;
private:
	const std::string	m_identifier;
};

}} // namespace _Wolframe::AAAA

#endif // _AUTHENTICATION_HPP_INCLUDED
