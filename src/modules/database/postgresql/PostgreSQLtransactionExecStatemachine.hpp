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
//\brief PostgreSQL interface to the standard database transaction execution statemechine
//\file PostgreSQLtransactionExecStatemachine.hpp
#ifndef _DATABASE_POSTGRESQL_LIBPQ_TRANSACTION_EXECUTION_STATEMACHINE_HPP_INCLUDED
#define _DATABASE_POSTGRESQL_LIBPQ_TRANSACTION_EXECUTION_STATEMACHINE_HPP_INCLUDED
#include "database/transactionExecStatemachine.hpp"
#include "database/statement.hpp"
#include "database/databaseError.hpp"
#include "system/objectPool.hpp"
#include <string>
#include <vector>
#include <cstdlib>
#include <boost/shared_ptr.hpp>
#include <libpq-fe.h>

namespace _Wolframe {
namespace db {

class PostgreSQLdbUnit;

//\class TransactionExecStatemachine_postgres
//\brief Implementation of the standard database transaction execution statemechine for postgresql (libpq)
//\remark The postgres connection is opened, closed, created and disposed by the caller
struct TransactionExecStatemachine_postgres :public TransactionExecStatemachine
{
	//\brief Constructor
	explicit TransactionExecStatemachine_postgres( PostgreSQLdbUnit* dbunit_);

	//\brief Destructor
	virtual ~TransactionExecStatemachine_postgres();

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
	//\brief Get the number of columns of the last result
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
	bool status( PGresult* res, State newstate);
	bool errorStatus( const std::string& message);
	bool executeInstruction( const char* stmstr, State newstate);

private:
	State m_state;
	PGresult* m_lastresult;
	boost::shared_ptr<db::DatabaseError> m_lasterror;
	Statement *m_statement;
	std::size_t m_nof_rows;
	std::size_t m_idx_row;
	bool m_hasResult;
	PostgreSQLdbUnit* m_dbunit;
	PoolObject<PGconn*>* m_conn;		//< DB connection
};

}}//namespace
#endif

