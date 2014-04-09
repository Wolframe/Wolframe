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
//\brief SQLite3 interface to the standard database transaction execution statemechine
//\file SQLiteTransactionExecStatemachine.hpp
#ifndef _DATABASE_SQLITE3_TRANSACTION_EXECUTION_STATEMACHINE_HPP_INCLUDED
#define _DATABASE_SQLITE3_TRANSACTION_EXECUTION_STATEMACHINE_HPP_INCLUDED
#include "database/transactionExecStatemachine.hpp"
#include "database/statement.hpp"
#include "system/objectPool.hpp"
#include <string>
#include <map>
#include <cstdlib>
#include <boost/shared_ptr.hpp>
#include "sqlite3.h"

namespace _Wolframe {
namespace db {

class SQLiteDBunit;

//\class TransactionExecStatemachine_sqlite3
//\brief Implementation of the standard database transaction execution statemechine for sqlite (Sqlite3)
//\remark The sqlite3 connection is opened, closed, created and disposed by the caller
struct TransactionExecStatemachine_sqlite3
	:public TransactionExecStatemachine
{
	//\brief Constructor
	explicit TransactionExecStatemachine_sqlite3( SQLiteDBunit* dbunit_);

	//\brief Destructor
	virtual ~TransactionExecStatemachine_sqlite3();

	//\brief Get the database identifier
	virtual const std::string& databaseID() const;

	//\brief Begin transaction
	virtual bool begin();
	//\brief Commit current transaction
	virtual bool commit();
	//\brief Rollback current transaction
	virtual bool rollback();

	//\brief Start new command statement
	virtual bool start( const std::string& statement);
	//\brief Bind parameter value on current command statement
	virtual bool bind( std::size_t idx, const types::VariantConst& value);
	//\brief Execute instance of current statement
	virtual bool execute();
	//\brief Return true is the last command has at least one result row returned
	virtual bool hasResult();
	//\brief Get the number of columns of the last result.
	//\remark This function should be used to check if the last command had a result (that can also be empty)
	virtual std::size_t nofColumns();
	//\brief Get a column title of the last result
	virtual const char* columnName( std::size_t idx);
	//\brief Get the last database error as string
	virtual const db::DatabaseError* getLastError();
	//\brief Get a column of the last result
	virtual types::VariantConst get( std::size_t idx);
	//\brief Skip to the next row of the last result
	virtual bool next();
	//\brief Find out if the database is case sensitive or not
	virtual bool isCaseSensitive()	{return false;}

private:
	void clear();
	bool firstResultIsNullRow() const;

	enum State
	{
		Init,
		Transaction,
		CommandReady,
		Executed,
		Error
	};
	static const char* stateName( State i)
	{
		const char* ar[] = {"Init","Transaction","CommandReady","Executed","Error"};
		return ar[ (int)i];
	}

	void setDatabaseErrorMessage();
	bool status( int rc, State newstate);
	bool errorStatus( const std::string& message);
	bool executeInstruction( const char* stmstr, State newstate);

private:
	State m_state;						//< command execution state
	std::string m_dbname;					//< name of database
	std::string m_curstm;					//< current statement executed
	bool m_hasResult;					//< last command executed is a command that returns a result set (the result might be empty, check 'm_hasRow' for  non empty result)
	bool m_hasRow;						//< last command executed result set has at least one result row
	boost::shared_ptr<db::DatabaseError> m_lasterror;	//< last error occurred
	sqlite3_stmt* m_stm;					//< current statement
	SQLiteDBunit* m_dbunit;					//< database unit
	PoolObject<sqlite3*>* m_conn;				//< database connection
	Statement *m_statement;					//< the statement parser
};

}}//namespace
#endif

