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
/// \file sqlitepp.hpp
/// \brief header file for the sqlite C++ layer
///

#include <string>
#include <stdexcept>

#include "sqlite3.h"

namespace _Wolframe {
	namespace db {
		namespace sqlite3pp {

	class db_error : std::runtime_error {
		public:
			explicit db_error( const std::string &err );

		
	};

	class result {
		public:
			result( );

			unsigned int rows_affected( );
			
	};

	class sql {
		public:
			sql( std::string &sql );
	};

	class connection;

	class transaction {
		public:

			// create a new transation in a given connection and
			// with a given name, if scoped define the default
			// action (commit or abort) at destruction time
			transaction( connection &c, const std::string &name, bool commit_on_destruct = true, bool implicit_begin = true );

			// create a new unnamed transaction (the library
			// creates a dummy transaction name)
			transaction( connection &c, bool commit_on_destruct = true, bool implicit_begin = true );

			~transaction( );

			// begin transaction explicitly
			void begin( );

			// end transaction
			void commit( );
			void rollback( );

			// execute a sql statement, checking results
			result exec( sql &s );

			// execute a string statement directly
			result exec( const std::string &sql );

		private:
			// back reference to the connection
			connection &m_connection;

			std::string m_name;

			bool m_commit_on_destruct;
			bool m_implicit_begin;

			typedef enum {
				TX_NASCENT,
				TX_ACTIVE,
				TX_COMMITTED,
				TX_ABORTED
			} tx_state;

			tx_state m_state;
	};

	class connection {
		public:
			// create connection (still in closed style)
			connection( );

			// create connection from a C layer handler
			connection( sqlite3 *handle );

			// create connection with a filename
			connection( const std::string &filename );

			// implicitly rollbacks all outstanding transactions
			// and closes the database file
			~connection( );

			// get internal handle of the Sqlite3 C layer
			sqlite3 *handle( );

			// open connection with a given filename
			void open( const std::string &filename );

			// open connection from existing sqlite3 C layer handle
			void open( sqlite3 *handle );

			// close connection, close outstanding transactions
			// (rollback), abort running sql statements
			void close( );

			// execute a sql statement
			result exec( sql &s );

			// execute a string statement directly
			result exec( const std::string &sql );

			friend class transaction;

		private:
			sqlite3 *m_db;
			bool m_db_extern;
			unsigned int m_trans_cnt;
	};

		} // namespace sqlite3pp
	} // namespace db
} // namespace _Wolframe
