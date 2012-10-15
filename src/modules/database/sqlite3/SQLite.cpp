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
#include "utils/miscUtils.hpp"
#include "SQLite.hpp"
#include "SQLitePreparedStatement.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include "sqlite3.h"

/*****  SQLite specialized template for PoolObject constructor  *******/
namespace _Wolframe {

template<>
PoolObject< sqlite3* >::PoolObject( ObjectPool< sqlite3* >& pool )
	: m_pool( pool ), m_object( pool.get() )
{
}

} // namespace _Wolframe


namespace _Wolframe {
namespace db {

SQLiteDBunit::SQLiteDBunit(const std::string& id,
			    const std::string& filename, bool flag,
			    unsigned short connections )
	: m_ID( id ), m_filename( filename ), m_flag( flag )
{
	bool	checked = false;

	for( int i = 0; i < connections; i++ ) {
		sqlite3 *handle;
		int res = sqlite3_open_v2( m_filename.c_str( ), &handle,
					   SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX,
#ifndef _WIN32
					   "unix"
#else
						NULL
#endif
					   );
		if( res != SQLITE_OK )	{
			MOD_LOG_ALERT << "Unable to open SQLite database '" << filename
				      << "': " << sqlite3_errmsg( handle );
				sqlite3_close( handle );	// really ?!?
			throw std::runtime_error( "Unable to open SQLite database" );
		}
		else	{
			if ( !checked )	{
				char* err;
//				res = sqlite3_exec( handle, "PRAGMA quick_check", NULL, &err );
				res = sqlite3_exec( handle, "PRAGMA integrity_check", NULL, NULL, &err );
				if( res != SQLITE_OK )	{
					MOD_LOG_ALERT << "Corrupt SQLite database '" << filename
						      << "': " << err;
					sqlite3_close( handle );
				}
				if( err )
					sqlite3_free( err );
				if( res != SQLITE_OK )
					throw std::runtime_error( "Corrupt SQLite database" );
				checked = true;
			}
			m_connections.push_back( handle );
			m_connPool.add( handle );
		}
	}
	m_db.setUnit( this );

	MOD_LOG_DEBUG << "SQLite database unit '" << m_ID << "' created with "
		      << connections << " connections to file '" << m_filename << "'";
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

void SQLiteDBunit::loadProgram( const std::string& filename )
{
	// No program file, do nothing
	if ( filename.empty())
		return;
	if ( !boost::filesystem::exists( filename ))	{
		MOD_LOG_ALERT << "Program file '" << filename
			      << "' does not exist (SQLite database '" << m_ID << "')";
		return;
	}
	std::string dbsource = utils::readSourceFileContent( filename);
	addProgram( dbsource);
}

Database* SQLiteDBunit::database()
{
	return m_db.hasUnit() ? &m_db : NULL;
}


/*****  SQLite database  **********************************************/
const std::string& SQLiteDatabase::ID() const
{
	if ( m_unit )
		return m_unit->ID();
	else
		throw std::runtime_error( "SQLite database unit not initialized" );
}

void SQLiteDatabase::loadProgram( const std::string& filename )
{
	if ( !m_unit )
		throw std::runtime_error( "loadProgram: SQLite database unit not initialized" );
	m_unit->loadProgram( filename );
}

void SQLiteDatabase::addProgram( const std::string& program )
{
	if ( !m_unit )
		throw std::runtime_error( "addProgram: SQLite database unit not initialized" );
	m_unit->addProgram( program );
}

const std::string* SQLiteDatabase::getProgram( const std::string& name) const
{
	if ( !m_unit )
		throw std::runtime_error( "getProgram: SQLite database unit not initialized" );
	return m_unit->getProgram( name );
}

Transaction* SQLiteDatabase::transaction( const std::string& /*name*/ )
{
	return new SQLiteTransaction( *this );
}

void SQLiteDatabase::closeTransaction( Transaction *t )
{
	delete t;
}


/*****  SQLite transaction  *******************************************/
SQLiteTransaction::SQLiteTransaction( SQLiteDatabase& database )
	: m_db( database ), m_unit( database.dbUnit() )
{
}

const std::string& SQLiteTransaction::databaseID() const
{
	return m_unit.ID();
}

void SQLiteTransaction::execute()
{
	try	{
		_Wolframe::PoolObject< sqlite3* > conn( m_unit.m_connPool);
		PreparedStatementHandler_sqlite3 ph( *conn, m_unit.stmmap());
		try
		{
			if (!ph.begin()
			||  !ph.doTransaction( m_input, m_output)
			||  !ph.commit())
			{
				const char* err = ph.getLastError();
				MOD_LOG_ERROR << "error in sqlite database transaction: " << (err?err:"unknown error");
				ph.rollback();
			}
		}
		catch (const std::runtime_error& e)
		{
			MOD_LOG_ERROR << "error in sqlite database transaction: " << e.what();
			ph.rollback();
		}
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


