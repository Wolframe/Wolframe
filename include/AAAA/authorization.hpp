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
///
/// \file authorization.hpp
/// \brief top-level header file for authorization interface
///

#ifndef _AUTHORIZATION_HPP_INCLUDED
#define _AUTHORIZATION_HPP_INCLUDED

#include <string>

#include "connectionEndpoint.hpp"
#include "database/DBprovider.hpp"

namespace _Wolframe {
namespace AAAA {

// interface for all authorization mechanisms
class Authorizer {
public:
	virtual ~Authorizer()		{}

	// close the authorizer (not really)
	virtual void close()		{}

	// authorization requests
	virtual bool connectAllowed( const net::LocalEndpoint& local, const net::RemoteEndpoint& remote,
				     std::string& msg ) = 0;
};


/// AuthorizationUnit Unit
/// This is the base class for authorization unit implementations
class AuthorizationUnit
{
public:
	enum Result	{
		DENIED,
		ALLOWED,
		IGNORED,
		ERROR
	};

	virtual ~AuthorizationUnit()	{}

	virtual const char* typeName() const = 0;

	virtual bool resolveDB( const db::DatabaseProvider& /*db*/ )
					{ return true; }

	virtual const char* name() const = 0;

	virtual Result connectAllowed( const net::LocalEndpoint& local,
				       const net::RemoteEndpoint& remote ) = 0;
};

}} // namespace _Wolframe::AAAA

#endif // _AUTHORIZATION_HPP_INCLUDED
