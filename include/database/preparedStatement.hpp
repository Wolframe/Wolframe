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
///\brief Interface to process prepared statements by a transaction function execute
///\file database/preparedStatement.hpp
#ifndef _DATABASE_PREPARED_STATEMENT_HPP_INCLUDED
#define _DATABASE_PREPARED_STATEMENT_HPP_INCLUDED
#include "database/transactionInput.hpp"
#include "database/transactionOutput.hpp"
#include "database/databaseError.hpp"
#include <string>
#include <cstdlib>

namespace _Wolframe {
namespace db {

///\class PreparedStatementHandler
///\brief Interface to process prepared statements in a database
class PreparedStatementHandler
{
public:
	///\brief Constructor
	PreparedStatementHandler(){}
	///\brief Destructor
	virtual ~PreparedStatementHandler(){}

	///\brief Start new command statement
	///\param[in] stmname name of prepared statement
	virtual bool start( const std::string& stmname)=0;
	///\brief Bind parameter value on current command statement
	virtual bool bind( std::size_t idx, const char* value)=0;
	///\brief Execute instance of current statement
	virtual bool execute()=0;
	///\brief Get the number of columns of the last result
	virtual std::size_t nofColumns()=0;
	///\brief Get a column title of the last result
	virtual const char* columnName( std::size_t idx)=0;
	///\brief Get a column of the last result
	virtual const char* get( std::size_t idx)=0;
	///\brief Skip to the next row of the last result
	virtual bool next()=0;
	///\brief Get the number of rows of the last result
	virtual bool hasResult()=0;
	///\brief Get the last database error as structure
	virtual const db::DatabaseError* getLastError()=0;

	///\brief Executes the transaction using the interface defined
	virtual bool doTransaction( const TransactionInput& input, TransactionOutput& output);
};

}}//namespace
#endif

