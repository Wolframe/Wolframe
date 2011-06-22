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
// database.cpp
//

#include "DBprovider.hpp"
#include "database/PostgreSQL.hpp"
#include "database/SQLite.hpp"

//#include "standardConfigs.hpp"
#include "logger.hpp"

#include <boost/algorithm/string.hpp>

namespace _Wolframe	{
namespace db	{

DatabaseProvider::DatabaseProvider( const DBproviderConfig& conf )
{
	for ( std::list<DatabaseConfig*>::const_iterator it = conf.m_dbConfig.begin();
							it != conf.m_dbConfig.end(); it++ )	{
		const char* dbType = (*it)->typeName();
		if ( boost::algorithm::iequals( dbType, "PostgreSQL" ))	{
			PostgreSQLcontainer* db = new PostgreSQLcontainer( static_cast<PostgreSQLconfig*>(*it) );
			m_db.push_back( db );
		}
		else if ( boost::algorithm::iequals( dbType, "SQLite" ))	{
			SQLiteContainer* db = new SQLiteContainer( static_cast<SQLiteConfig*>(*it) );
			m_db.push_back( db );
		}
		else if ( boost::algorithm::iequals( dbType, "DB reference" ))
			throw std::domain_error( "Database reference in DBprovider constructor" );
		else
			throw std::domain_error( "Unknown database type in DBprovider constructor" );
	}
}

DatabaseProvider::~DatabaseProvider()
{
	for ( std::list< DatabaseContainer* >::const_iterator it = m_db.begin();
							it != m_db.end(); it++ )
		delete *it;
}


const Database* DatabaseProvider::database( const std::string& id ) const
{
	for ( std::list< DatabaseContainer* >::const_iterator it = m_db.begin();
							it != m_db.end(); it++ )	{
		if ( (*it)->ID() == id )
			return &(*it)->database();
	}
	return NULL;
}

}} // namespace _Wolframe::db
