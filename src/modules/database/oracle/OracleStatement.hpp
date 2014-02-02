/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file database/OracleStatement.hpp
#ifndef _ORACLE_STATEMENT_HPP_INCLUDED
#define _ORACLE_STATEMENT_HPP_INCLUDED
#include <string>
#include <vector>
#include "Oracle.hpp"
#include "database/baseStatement.hpp"

namespace _Wolframe {
namespace db {

class OracleStatement : public BaseStatement
{
	public:
		OracleStatement( );
		OracleStatement( const OracleStatement &o );
		OracleStatement( OracleEnvirenment *env );

		virtual void bind( const unsigned int idx, const types::Variant &value );

		virtual const std::string replace( const unsigned int idx ) const;

		void setConnection( OracleConnection *conn );
		void setStatement( OCIStmt *stmt );
		
		sword getLastStatus( );

	private:
		void bindUInt( const unsigned int idx, const unsigned int &value );
		void bindInt( const unsigned int idx, const int &value );
		void bindBool( const unsigned int idx, const bool &value );
		void bindDouble( const unsigned int idx, const double &value );
		void bindString( const unsigned int idx, const char* value, const std::size_t size );
		void bindNull( const unsigned int idx );
	
	private:
		OracleEnvirenment *m_env;
		OracleConnection *m_conn;
		OCIStmt *m_stmt;
		sword m_status;
		std::vector<types::Variant> m_data;
};

}}//namespace
#endif

