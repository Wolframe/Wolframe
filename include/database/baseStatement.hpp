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
///\file database/statement.hpp

#ifndef _DATABASE_BASE_STATEMENT_HPP_INCLUDED
#define _DATABASE_BASE_STATEMENT_HPP_INCLUDED

#include "database/statement.hpp"

#include <vector>
#include <bitset>

namespace _Wolframe {
namespace db {

///\class BaseStatement
///\brief Implements basic parsing of the Wolframe SQL statement with
///       placeholders, derived classes can steer how the SQL string is
///       converted to native format
class BaseStatement : public Statement
{
	public:
		BaseStatement( );
		BaseStatement( const BaseStatement &o );
		explicit BaseStatement( const std::string &stmtStr );
		
		virtual void init( const std::string &stmtStr );

		virtual void clear( );
		
		virtual void substitute( bool withPlaceholders = true );

		virtual void bind( const unsigned int idx, const types::Variant &value );

		virtual const std::string originalSQL( ) const;

		virtual const std::string nativeSQL( ) const;
		
	protected:
		/// \brief The original SQL
		std::string m_stmtStr;

		/// \brief The maximum number of parsed placeholders so far
		unsigned int m_maxParam;

	private:
		void parse( );

	public:
		/// \enum MaxNofParams
		/// \brief this implementation can handle at most MaxNofParams
		///        placeholders. The code checks the limit and throws
		///        an exception if more parameters are used. Derived
		///        classes should use the same constant.
		enum { MaxNofParams = 32 };
		
	private:
		typedef std::pair<unsigned int, std::string> Element;

		/// \brief Remembers positions and parts of SQL snippets
		std::vector<Element> m_data;

		/// \brief The SQL statement as understood by the database
		std::string m_nativeStmt;

		/// \brief Remembers the indexes used in the SQL statement
		std::bitset<MaxNofParams> m_usedIdx;

		/// \brief Remembers the placeholders bound to actual data
		std::bitset<MaxNofParams> m_setIdx;
};

}}//namespace
#endif

