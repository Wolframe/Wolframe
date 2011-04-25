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

///
/// \file sqlitepp.cpp
/// \brief implementation of the sqlite C++ layer
///

#include "db/sqlite3/sqlite3pp.hpp"

#include <sstream>

namespace _Wolframe {
	namespace db {
		namespace sqlite3pp {

// db_error

db_error::db_error( const std::string &err ) : std::runtime_error( err )
{
}

// result

result::result( )
	: m_stmt( 0 )
{
}

result::~result( )
{
	if( m_stmt )
		sqlite3_finalize( m_stmt );
}

unsigned int result::rows_affected( )
{
	if( m_stmt )
		return sqlite3_changes( sqlite3_db_handle( m_stmt ) );
	else
		throw db_error( "rows_affected() called with illegal statement" );
}

// connection

connection::connection( )
	: m_db( 0 ), m_db_extern( false ), m_trans_cnt( 0 )
{
}

connection::connection( const std::string &filename )
	: m_db( 0 ), m_db_extern( false ), m_trans_cnt( 0 )
{
	open( filename );
}

connection::connection( sqlite3 *h )
	: m_db( 0 ), m_db_extern( false ), m_trans_cnt( 0 )
{
	open( h );
}

connection::~connection( )
{
	close( );
}

void connection::open( const std::string &filename )
 {
	int res;

	res = sqlite3_open( filename.c_str( ), &m_db );
	if( res != SQLITE_OK )
		throw db_error( sqlite3_errmsg( m_db ) );
	if( !m_db )
		throw db_error( "Out of memory, got NULL as handle!" );

	m_db_extern = false;
}

void connection::open( sqlite3 *h )
{
	m_db = h;
	m_db_extern = true;
}

void connection::close( )
{
	if( m_db && !m_db_extern ) {
		sqlite3_close( m_db );
		m_db = 0;
	}
}

sqlite3 *connection::handle( )
{
	return m_db;
}

result connection::exec( const std::string &sql )
{
	int res;
	const char *tail;
	result result;

#if SQLITE_VERSION_NUMBER >= 3005000
	res = sqlite3_prepare_v2( m_db, sql.c_str( ), -1, &result.m_stmt, &tail );
#else
	res = sqlite3_prepare( m_db, sql.c_str( ), -1, &result.m_stmt, &tail );
#endif
	if( res != SQLITE_OK ) 
		throw db_error( sqlite3_errmsg( m_db ) );

	res = sqlite3_step( result.m_stmt );
	if( res == SQLITE_DONE ) {
	} else if( res == SQLITE_ROW ) {
	} else if( res == SQLITE_BUSY ) {
		// TODO: if in a transaction, do a rollback
	} else {
		throw db_error( sqlite3_errmsg( m_db ) );
	}

	return result;
}

// transaction

transaction::transaction( connection &c, const std::string &name, bool commit_on_destruct, bool implicit_begin )
	: m_connection( c ), m_name( name ),
	  m_commit_on_destruct( commit_on_destruct ),
	  m_implicit_begin( implicit_begin ),
	  m_state( TX_NASCENT )
{
}

transaction::transaction( connection &c, bool commit_on_destruct, bool implicit_begin )
	: m_connection( c ),
	  m_commit_on_destruct( commit_on_destruct ),
	  m_implicit_begin( implicit_begin ),
	  m_state( TX_NASCENT )
{
	std::ostringstream ss;
	c.m_trans_cnt++;
	ss << "trans_" << c.m_trans_cnt;
	m_name = ss.str( );

	if( implicit_begin )
		begin( );
}

transaction::~transaction( )
{
	if( m_state == TX_ACTIVE ) {
		if( m_commit_on_destruct )
			commit( );
		else
			rollback( );
	}
}

result transaction::exec( const std::string &sql )
{
	if( m_state != TX_ACTIVE )
		throw db_error( "exec('" + sql + "') unexpected at this time" );

	return m_connection.exec( sql );
}

void transaction::begin( )
{
	if( m_state == TX_NASCENT ) {
		m_state = TX_ACTIVE;
		exec( "begin" );
	} else {
		throw db_error( "begin() unexpected at this time" );
	}
}

void transaction::commit( )
{
	if( m_state == TX_ACTIVE ) {
		exec( "commit" );
		m_state = TX_COMMITTED;
	} else {
		throw db_error( "commit() unexpected at this time" );
	}
}

void transaction::rollback( )
{
	if( m_state == TX_ACTIVE ) {
		exec( "rollback" );
		m_state = TX_ABORTED;
	} else {
		throw db_error( "rollback() unexpected at this time" );
	}
}

		} // namespace sqlite3pp
	} // namespace db
} // namespace _Wolframe
