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
///\brief Interface to substitute parameters in embedded SQL statements
///\file database/SQLiteStatement.hpp
#ifndef _SQLITE_STATEMENT_HPP_INCLUDED
#define _SQLITE_STATEMENT_HPP_INCLUDED
#include <string>
#include <vector>
#include "database/baseStatement.hpp"
#include "sqlite3.h"

namespace _Wolframe {
namespace db {

class SQLiteStatement : public BaseStatement
{
	public:
		SQLiteStatement( );
		SQLiteStatement( const SQLiteStatement &o );

		virtual void clear( );

		virtual void bind( const unsigned int idx, const types::Variant &arg );

		virtual const std::string replace( const unsigned int idx ) const;

		void setStatement( sqlite3_stmt *stm );
		
		int getLastStatus( );
	
	private:
		///\brief Implementation of bind(const unsigned int,const types::Variant&) without boundary checking
		void bindVariant( unsigned int idx, const types::Variant &value);

	private:
		sqlite3_stmt *m_stm;
		int m_rc;
		std::vector<std::string> m_data;
};

}}//namespace
#endif

