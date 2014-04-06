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
///\brief Interface to the standard database transaction execution statemechine
///\file database/transactionExecStatemachine.hpp
#ifndef _DATABASE_TRANSACTION_EXECUTION_STATEMACHINE_HPP_INCLUDED
#define _DATABASE_TRANSACTION_EXECUTION_STATEMACHINE_HPP_INCLUDED
#include "database/databaseError.hpp"
#include "types/variant.hpp"
#include <string>
#include <cstdlib>

namespace _Wolframe {
namespace db {

///\class TransactionExecStatemachine
///\brief Interface to the database transaction execution statemechine.
///\remark TODO: Provide state transition schema
class TransactionExecStatemachine
{
public:
	///\brief Constructor
	explicit TransactionExecStatemachine( const std::string& name_)
		:m_name(name_){}
	///\brief Destructor
	virtual ~TransactionExecStatemachine(){}

	///\brief Get database identifier
	virtual const std::string& databaseID() const=0;

	///\brief Begin transaction
	virtual bool begin()=0;
	///\brief Commit current transaction
	virtual bool commit()=0;
	///\brief Rollback current transaction
	virtual bool rollback()=0;

	///\brief Start new command statement
	///\param[in] statement statement string
	virtual bool start( const std::string& statement)=0;
	///\brief Bind parameter value on current command statement
	virtual bool bind( std::size_t idx, const types::VariantConst& value)=0;
	///\brief Execute instance of current statement
	virtual bool execute()=0;
	///\brief Get the number of columns of the last result
	virtual std::size_t nofColumns()=0;
	///\brief Get a column title of the last result
	virtual const char* columnName( std::size_t idx)=0;
	///\brief Get a column of the last result
	virtual types::VariantConst get( std::size_t idx)=0;
	///\brief Skip to the next row of the last result
	virtual bool next()=0;
	///\brief Evaluate if there is a result
	virtual bool hasResult()=0;
	///\brief Get the last database error as structure
	virtual const db::DatabaseError* getLastError()=0;
	///\brief Find out if the database is case sensitive or not
	virtual bool isCaseSensitive()=0;

private:
	const std::string m_name;
};

typedef boost::shared_ptr<TransactionExecStatemachine> TransactionExecStatemachineR;

}}//namespace
#endif

