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
/// \brief top-level header file for authentication unit interface
///

#ifndef _AUTHENTICATION_UNIT_HPP_INCLUDED
#define _AUTHENTICATION_UNIT_HPP_INCLUDED

#include <string>

#include "AAAA/authInstance.hpp"
#include "database/DBprovider.hpp"
#include "system/connectionEndpoint.hpp"

namespace _Wolframe {
namespace AAAA {

/// AuthenticationUnit Unit
/// This is the base class for the authentication unit implementations
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

	/// The list of mechs implemented by this unit
	/// \note	The authentication unit returns the mechs as an
	///		array of strings. The array ends with an empty string.
	///		Be aware that the other interfaces use a vector instead.
	virtual const std::string* mechs() const = 0;

	/// An AuthenticatorInstance for the required mech (or NULL)
	virtual AuthenticatorInstance* instance( const std::string& mech,
						 const net::RemoteEndpoint& client ) = 0;

private:
	const std::string	m_identifier;
};

}} // namespace _Wolframe::AAAA

#endif // _AUTHENTICATION_UNIT_HPP_INCLUDED
