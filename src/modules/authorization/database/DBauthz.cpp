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
//
//
//

#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include "logger-v1.hpp"
#include "DBauthz.hpp"

namespace _Wolframe {
namespace AAAA {

//***  Database authorizer  *********************************************
DBauthorizer::DBauthorizer( const std::string& Identifier, const std::string& DbLabel )
	: AuthorizationUnit( Identifier ), m_dbLabel( DbLabel )
{
	m_db = NULL;
	if ( m_dbLabel.empty() )
		throw std::logic_error( "Empty database reference in DBauthorizer" );

	LOG_DEBUG << "Database authorizer created with database reference '" << m_dbLabel << "'";
}

DBauthorizer::~DBauthorizer()
{
}


bool DBauthorizer::resolveDB( const db::DatabaseProvider& db )
{
	if ( m_db == NULL && ! m_dbLabel.empty() )	{
		m_db = db.database( m_dbLabel );
		if ( m_db )	{
			LOG_TRACE << "Database authorizer: database reference '" << m_dbLabel << "' resolved";
			return true;
		}
		else	{
			LOG_ERROR << "Database authorizer: database labeled '" << m_dbLabel << "' not found !";
			return false;
		}
	}
	return true;
}


AuthorizationUnit::Result DBauthorizer::allowed( const Information& authzObject )
{
	switch ( authzObject.type() )	{
		case Information::CONNECTION:	{
			const ConnectInfo& obj = dynamic_cast<const ConnectInfo&>( authzObject );
			return connectionAllowed( obj.local, obj.remote);
		}
		case Information::LOGIN:
		case Information::LOGOUT:
		case Information::TRANSACTION:
			return RES_IGNORED;
	}
	return RES_ERROR;
}

AuthorizationUnit::Result DBauthorizer::connectionAllowed( const net::LocalEndpoint& /*local*/,
							   const net::RemoteEndpoint& remote )
{
	// that's just for testing - allow localhost only
	if ( remote.host() == "127.0.0.1" )
		return RES_ALLOWED;
	return RES_DENIED;
}

}} // namespace _Wolframe::AAAA

