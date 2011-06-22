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
// Wolframe PostgreSQL client view implementation
//

#include "database/PostgreSQL.hpp"

namespace _Wolframe {
namespace db {

//***  PostgreSQL configuration functions  **********************************
PostgreSQLconfig::PostgreSQLconfig( const char* cfgName, const char* logParent, const char* logName )
	: DatabaseConfig( cfgName, logParent, logName )
{
	port = 0;
	connections = 0;
	acquireTimeout = 0;
}

void PostgreSQLconfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );

	os << indStr << sectionName() << ":" << std::endl;
	if ( ! ID().empty() )
		os << indStr << "   ID: " << ID() << std::endl;
	if ( host.empty())
		os << indStr << "   Database host: local unix domain socket" << std::endl;
	else
		os << indStr << "   Database host: " << host << ":" << port << std::endl;
	os << indStr << "   Database name: " << (dbName.empty() ? "(not specified - server user default)" : dbName) << std::endl;
	os << indStr << "   Database user: " << (user.empty() ? "(not specified - same as server user)" : user)
	   << ", password: " << (password.empty() ? "(not specified - no password used)" : password) << std::endl;
	os << indStr << "   Database connections: " << connections << std::endl;
	os << indStr << "   Acquire database connection timeout: " << acquireTimeout << std::endl;
}

bool PostgreSQLconfig::check() const
{
	if ( connections == 0 )	{
		LOG_ERROR << logPrefix() << "number of database connections cannot be 0";
		return false;
	}
	return true;
}


//***  PostgreSQL database functions  ***************************************
PostgreSQLdatabase::PostgreSQLdatabase( const std::string& id, const std::string& host,
					unsigned short port, const std::string& dbName,
					const std::string& user, const std::string& password,
					unsigned short connections, unsigned short acquireTimeout )
	: m_ID( id )
{
	LOG_NOTICE << "PostgreSQL database '" << m_ID << "' created with "
		   << "server " << host << ":" << port << ", database '" << dbName
		   << "', credentials: " << user << "/" << password
		   << ", " << connections << " connections, " << acquireTimeout << "s timeout";
}

PostgreSQLdatabase::~PostgreSQLdatabase()
{
}


PostgreSQLcontainer::PostgreSQLcontainer( const PostgreSQLconfig* conf )
	: m_db( conf->ID(), conf->host, conf->port, conf->dbName,
		  conf->user, conf->password,
		  conf->connections, conf->acquireTimeout )
{
	LOG_NOTICE << "PostgreSQL database container for '" << conf->ID() << "' created";
}

PostgreSQLcontainer::~PostgreSQLcontainer()
{
}

}} // _Wolframe::db

