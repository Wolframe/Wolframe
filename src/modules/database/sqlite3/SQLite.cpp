/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
// Wolframe SQLite client view implementation
//

#include "logger-v1.hpp"
#include "SQLite.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "utils/miscUtils.hpp"

#include "sqlite3.h"

namespace _Wolframe {
namespace db {

SQLiteDBunit::SQLiteDBunit( const std::string& id,
			    const std::string& filename, unsigned short connections, bool flag )
	: m_ID( id ), m_filename( filename ), m_flag( flag )
{
	for( int i = 0; i < connections; i++ ) {
		sqlite3 *handle;
		int res = sqlite3_open( m_filename.c_str( ), &handle );
		if( res != SQLITE_OK ) {
			MOD_LOG_ERROR << "Unable to open Sqlite3 database '" << filename << ": " << sqlite3_errmsg( handle );
		}

		m_connections.push_back( handle );
		m_connPool.add( handle );
	}

	m_db.setUnit( this );

	MOD_LOG_DEBUG << "SQLite database '" << m_ID << "' created with "
		      << "filename '" << m_filename << "'";
}

SQLiteDBunit::~SQLiteDBunit( )
{
	m_db.setUnit( NULL );

	while( m_connPool.available( ) > 0 ) {
		sqlite3 *handle = m_connPool.get( );
		sqlite3_close( handle );
	}
	MOD_LOG_TRACE << "SQLite database unit '" << m_ID << "' destroyed";
}


/*****  SQLite database  **********************************************/
Database* SQLiteDBunit::database()
{
	return m_db.hasUnit() ? &m_db : NULL;
}

const std::string& SQLiteDatabase::ID() const
{
	if ( m_unit )
		return m_unit->ID();
	else
		throw std::runtime_error( "SQL database unit not initialized" );
}

Transaction* SQLiteDatabase::transaction( const std::string& /*name*/ )
{
	return new SQLiteTransaction( *this );
}

void SQLiteDatabase::closeTransaction( const Transaction *t ) const
{
	delete t;
}

/*****  SQLite transaction  *******************************************/
SQLiteTransaction::SQLiteTransaction( SQLiteDatabase& database )
	: m_db( database ), m_unit( *database.m_unit )
{
}

const std::string& SQLiteTransaction::databaseID() const
{
	return m_unit.ID();
}

void SQLiteTransaction::execute()
{
	try	{
//		_Wolframe::PoolObject< sqlite3* > conn( m_unit.m_connPool );
//		int err = sqlite3_errcode( *conn );
//		MOD_LOG_DEBUG << "SQLite error code: " << err;
	}
	catch ( _Wolframe::ObjectPoolTimeout )
	{
	}
}

void SQLiteTransaction::close()
{
	m_db.closeTransaction( this );
}

}} // _Wolframe::db


