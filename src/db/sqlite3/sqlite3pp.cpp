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

namespace _Wolframe {
	namespace db {
		namespace sqlite3pp {

// db_error

db_error::db_error( const std::string &err ) : std::runtime_error( err )
{
}

// result

result::result( )
{
}

// connection

connection::connection( ) : m_db( 0 ), m_db_extern( false )
{
}

connection::connection( const std::string &filename )
{
	open( filename );
}

connection::connection( sqlite3 *h )
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

result connection::exec( const std::string &sql )
{
	int res;
	result noresult;

	res = sqlite3_exec( m_db, sql.c_str( ), 0, 0, NULL );
	if( res != SQLITE_OK )
		throw db_error( sqlite3_errmsg( m_db ) );

	return noresult;
}

		} // namespace sqlite3pp
	} // namespace db
} // namespace _Wolframe
