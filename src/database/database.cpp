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

#include "database/database.hpp"
#include "database/PostgreSQL.hpp"

//#include "standardConfigs.hpp"
#include "logger.hpp"

#include <boost/algorithm/string.hpp>

namespace _Wolframe	{
namespace db	{

DatabaseType Database::strToType( const char *str )
{
	if ( boost::algorithm::iequals( str, "PostgreSQL" ))	return DBTYPE_POSTGRESQL;
	else if ( boost::algorithm::iequals( str, "SQLite" ))	return DBTYPE_SQLITE;
	else if ( boost::algorithm::iequals( str, "Reference" ))return DBTYPE_REFERENCE;
	else
		throw std::domain_error( "Unknown database type in Database::strToType(...)" );
}

std::string& Database::typeToStr( DatabaseType type )
{
	static std::string	retVal[ 3 ] = { "Reference to database", "PostgreSQL", "SQLite" };
	switch ( type )	{
	case DBTYPE_REFERENCE:	return retVal[0];
	case DBTYPE_POSTGRESQL:	return retVal[1];
	case DBTYPE_SQLITE:	return retVal[2];
	default:
		throw std::domain_error( "Unknown database type in Database::typeToStr(...)" );
	}
}


SQLiteDatabase::SQLiteDatabase( const SQLiteConfig* conf )
	: Database( conf->ID())
{
	LOG_NOTICE << "SQLite database '" << conf->ID() << "' created";
}


DBprovider::DBprovider( const Configuration& conf )
{
	for ( std::list<DatabaseConfig*>::const_iterator it = conf.m_dbConfig.begin();
							it != conf.m_dbConfig.end(); it++ )	{
		switch( (*it)->type() )	{
		case DBTYPE_POSTGRESQL:	{
			PostgreSQLDatabase* db = new PostgreSQLDatabase( static_cast<PostgreSQLconfig*>(*it) );
			m_db.push_back( db );
		}
			break;
		case DBTYPE_SQLITE:	{
			SQLiteDatabase* db = new SQLiteDatabase( static_cast<SQLiteConfig*>(*it) );
			m_db.push_back( db );
		}
			break;
		case DBTYPE_REFERENCE:
			throw std::domain_error( "Database reference in DBprovider constructor" );
		default:
			throw std::domain_error( "Unknown database type in DBprovider constructor" );
		}
	}
}

DBprovider::~DBprovider()
{
	for ( std::list< Database* >::const_iterator it = m_db.begin();
							it != m_db.end(); it++ )
		delete *it;
}


const Database* DBprovider::database( std::string& id ) const
{
	for ( std::list< Database* >::const_iterator it = m_db.begin();
							it != m_db.end(); it++ )	{
		if ( (*it)->ID() == id )
			return *it;
	}
	return NULL;
}

}} // namespace _Wolframe::db
