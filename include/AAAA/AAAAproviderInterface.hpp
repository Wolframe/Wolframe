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
/// \file AAAA/AAAAproviderInterface.hpp
/// \brief AAAA provider interface

#ifndef _AAAA_PROVIDER_INTERFACE_HPP_INCLUDED
#define _AAAA_PROVIDER_INTERFACE_HPP_INCLUDED

#include "authenticator.hpp"
#include "passwordChanger.hpp"
#include "authorization.hpp"
#include "audit.hpp"
#include "user.hpp"
#include "system/connectionEndpoint.hpp"

namespace _Wolframe {
namespace AAAA {

/// \class AAAAproviderInterface
/// \brief Provider interface to create AAAA related objects
class AAAAproviderInterface
{
public:
	/// \brief Destructor
	virtual ~AAAAproviderInterface(){}

	/// \brief Create an return an authenticator object
	virtual Authenticator* authenticator( const net::RemoteEndpoint& client) const=0;
	/// \brief Create an return a password changer object
	virtual PasswordChanger* passwordChanger( const User& user,
					  const net::RemoteEndpoint& client ) const=0;
	/// \brief Create an return an authorizer object
	virtual Authorizer* authorizer() const=0;
	/// \brief Create an return an auditor object
	virtual Auditor* auditor() const=0;
};

}}// namespace
#endif

