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
// Wolframe SQLite client view implementation
//

#include "logger-v1.hpp"
#include "SQLite.hpp"
#include "SQLiteTransactionExecStatemachine.hpp"
#include "utils/fileUtils.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "sqlite3.h"

namespace _Wolframe {
namespace db {

extern "C" void profiling_callback(  void * /*a*/, const char *b, sqlite3_uint64 c )
{
	LOG_DATA << b << " (time: " << c / 1000 << " ms)";
}

SQLiteDBunit::SQLiteDBunit( const std::string& id, const std::string& filename,
			    bool foreignKeys, bool profiling,
			    unsigned short connections,
			    const std::vector<std::string>& extensionFiles_)
	: m_ID( id ), m_filename( filename ),
	m_extensionFiles(extensionFiles_)
{
	bool	checked = false;
	int	dbFlags = SQLITE_OPEN_READWRITE;

	if ( ! sqlite3_threadsafe() )	{
		if ( connections != 1 )	{
			LOG_WARNING << "SQLite database '" << id
				    << "' has not been compiled without the SQLITE_THREADSAFE parameter."
				    << " Using only 1 connection instead of " << connections << ".";
			connections = 1;
		}
		else	{
			if ( sqlite3_config( SQLITE_CONFIG_MULTITHREAD ) != SQLITE_OK )	{
				throw std::runtime_error( "Unable to set SQLite in multithreaded mode" );
			}
			dbFlags |= SQLITE_OPEN_NOMUTEX;
//			dbFlags |= SQLITE_OPEN_FULLMUTEX;
		}
	}

	for( int i = 0; i < connections; i++ ) {
		sqlite3 *handle;
		char* err;
		int res = sqlite3_open_v2( m_filename.c_str( ), &handle, dbFlags,
#ifndef _WIN32
					   "unix" );
#else
					   NULL );
#endif
		if( res != SQLITE_OK )	{
			LOG_ALERT << "Unable to open SQLite database '" << filename
				      << "': " << sqlite3_errmsg( handle );
				sqlite3_close( handle );	// really ?!?
			throw std::runtime_error( "Unable to open SQLite database" );
		}
		else	{
			if ( !checked )	{
				res = sqlite3_exec( handle, "PRAGMA integrity_check", NULL, NULL, &err );
				if( res != SQLITE_OK )	{
					LOG_ALERT << "Corrupt SQLite database '" << filename
						      << "': " << err;
					sqlite3_close( handle );
				}
				if( err )
					sqlite3_free( err );
				if( res != SQLITE_OK )
					throw std::runtime_error( "Corrupt SQLite database" );
				checked = true;
			}

			// enable foreign keys
			if ( foreignKeys )	{
				res = sqlite3_exec( handle, "PRAGMA foreign_keys=true", NULL, NULL, &err );
				if( res != SQLITE_OK ) {
					LOG_ALERT << "Unable to enforce integrity checks in '" << filename
						      << "': " << err;
				}
				if( err ) {
					sqlite3_free( err );
				}
			}
			// enable tracing and profiling of commands
			if ( profiling )
				sqlite3_profile( handle, profiling_callback, NULL );

			// enable extensions in every connection
			std::vector<std::string>::const_iterator it = m_extensionFiles.begin( ), end = m_extensionFiles.end( );
			for( ; it != end; it++ ) {
				LOG_DEBUG << "Loading extension '" << *it << "' for SQLite database unit '" << m_ID << "'";
				// No extension file, do nothing
				if( (*it).empty( ) ) continue;

				if( !boost::filesystem::exists( (*it) ) ) {
					LOG_ALERT << "Extension file '" << (*it) << "' does not exist (SQLite database '" << m_ID << "')";
					continue;
				}

				// turn loading of extensions on, we expect the administrator to know
				// what he is doing if he puts an 'extension' directive in the code
				sqlite3_enable_load_extension( handle, 1 );

				char *errmsg;
				int rc = sqlite3_load_extension( handle, (*it).c_str( ), 0, &errmsg );
				if( rc != SQLITE_OK ) {
					LOG_ALERT << "Unable to load SQLite extension '" << (*it)
						      << "': " << errmsg;
					sqlite3_free( errmsg );
					// Aba, TOOD: throw here?
					continue;
				}
			}
			LOG_DEBUG << "Extensions for SQLite database unit '" << m_ID << "' loaded";

			m_connections.push_back( handle );
			m_connPool.add( handle );
		}
	}
	m_db.setUnit( this );

	LOG_DEBUG << "SQLite database unit '" << m_ID << "' created with "
		      << connections << " connections to file '" << m_filename << "'";
}

SQLiteDBunit::~SQLiteDBunit( )
{
	m_db.setUnit( NULL );

	while( m_connPool.available( ) > 0 ) {
		sqlite3 *handle = m_connPool.get( );
		sqlite3_close( handle );
	}
	LOG_TRACE << "SQLite database unit '" << m_ID << "' destroyed";
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

Transaction* SQLiteDatabase::transaction( const std::string& name)
{
	return new SQLiteTransaction( *this, name);
}

void SQLiteDatabase::closeTransaction( Transaction *t )
{
	delete t;
}


/*****  SQLite transaction  *******************************************/
SQLiteTransaction::SQLiteTransaction( SQLiteDatabase& database, const std::string& name_)
	:Transaction( name_, TransactionExecStatemachineR( new TransactionExecStatemachine_sqlite3( &database.dbUnit()))){}


}} // _Wolframe::db
