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
/// \file authorization.hpp
/// \brief top-level header file for authorization interface
///

#ifndef _AUTHORIZATION_HPP_INCLUDED
#define _AUTHORIZATION_HPP_INCLUDED

#include <string>

#include "AAAAinformation.hpp"
#include "system/connectionEndpoint.hpp"
#include "database/DBprovider.hpp"

namespace _Wolframe {
namespace AAAA {

/// Interface for all authorization mechanisms
class Authorizer {
public:
	virtual ~Authorizer()		{}

	// close the authorizer (not really)
	virtual void close()		{}

	// authorization requests
	virtual bool allowed( const Information& ) = 0;
};


/// AuthorizationUnit Unit
/// This is the base class for authorization unit implementations
class AuthorizationUnit
{
public:
	enum Result	{
		AUTHZ_DENIED,
		AUTHZ_ALLOWED,
		AUTHZ_IGNORED,
		AUTHZ_ERROR
	};

	AuthorizationUnit( const std::string& Identifier )
		: m_identifier( Identifier )	{}

	virtual ~AuthorizationUnit()		{}

	const std::string& identifier() const	{ return m_identifier; }

	virtual bool resolveDB( const db::DatabaseProvider& /*db*/ )
						{ return true; }
	virtual const char* className() const = 0;

	virtual Result allowed( const Information& ) = 0;

private:
	void operator=( const AuthorizationUnit&){}
private:
	const std::string	m_identifier;
};

}} // namespace _Wolframe::AAAA

#endif // _AUTHORIZATION_HPP_INCLUDED
