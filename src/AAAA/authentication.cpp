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
// authentication.cpp
//

#include "AAAA/authentication.hpp"

namespace _Wolframe {
namespace AAAA {

AuthenticatorFactory::AuthenticatorFactory( )
{
}

AuthenticatorFactory::~AuthenticatorFactory( )
{
	std::map<std::string, Authenticator *>::iterator it;
	for( it = m_authenticators.begin( ); it != m_authenticators.end( ); it++ )
		delete it->second;
}

void AuthenticatorFactory::registerAuthenticator(	std::string _method,
							CreateAuthenticatorFunc _createf,
							properties _props )
{
	m_authenticators.insert( std::make_pair<std::string, Authenticator *>( _method, _createf( _props ) ) );
}

void AuthenticatorFactory::unregisterAuthenticator( std::string _method )
{
	m_authenticators.erase( _method );
}

Authenticator* AuthenticatorFactory::getAuthenticator( const std::string method )
{
	std::map<std::string, Authenticator *>::const_iterator it = m_authenticators.find( method );
	if( it != m_authenticators.end( ) )
		return it->second;

	return 0;
}

std::vector<std::string> AuthenticatorFactory::getAvailableMechs( )
{
	std::vector<std::string> mechs;
	std::map<std::string, Authenticator *>::const_iterator it;
	for( it = m_authenticators.begin( ); it != m_authenticators.end( ); it++ )
		mechs.push_back( it->first );
	return mechs;
}

}} // namespace _Wolframe::AAAA

