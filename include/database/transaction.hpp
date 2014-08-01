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
/// \file database/transaction.hpp
/// \brief Interface of a database transaction

#ifndef _TRANSACTION_HPP_INCLUDED
#define _TRANSACTION_HPP_INCLUDED
#include "database/vmTransactionInput.hpp"
#include "database/vmTransactionOutput.hpp"
#include "database/transactionExecStatemachine.hpp"
#include "database/databaseError.hpp"
#include "types/variant.hpp"
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace db {

/// \class Transaction
/// \brief Transaction interface
class Transaction
{
public:
	/// \brief Constructor
	/// \param[in] name_ name of the transaction (for logging purpose only)
	/// \param[in] stm_ database interface needed by the virtual machine to execute the transaction
	Transaction( const std::string& name_, const TransactionExecStatemachineR& stm_)
		:m_name(name_),m_stm(stm_){}

	/// \brief Destructor
	virtual ~Transaction()			{close();}
	/// \brief Configured ID of the underlaying database
	const std::string& databaseID() const	{return m_stm->databaseID();}

	/// \brief Begin of a new transaction
	void begin();
	/// \brief Commit of the running transaction
	void commit();
	/// \brief Rollback of the running transaction
	void rollback();
	/// \brief Close of the committed or rolled back transaction
	void close()				{m_stm.reset();}

	/// \brief Execute a transaction
	/// \return true if successful, otherwise false (use getLastError to get details)
	virtual bool execute( const VmTransactionInput& input, VmTransactionOutput& output);

	/// \brief Get the name of the transaction
	const std::string& name() const
	{
		return m_name;
	}

	/// \class Result
	/// \brief Result of a single statement execute call: executeStatement( const std::string&, const std::vector<types::Variant>&);
	class Result
	{
	public:
		typedef std::vector<types::Variant> Row;

		/// \brief Default constructor
		Result(){}
		/// \brief Constructor
		/// \param[in] colnames_ column names of the result
		/// \param[in] rows_ data rows of the result
		Result( const std::vector<std::string>& colnames_, const std::vector<Row>& rows_)
			:m_colnames(colnames_),m_rows(rows_){}
		/// \brief Copy constructor
		Result( const Result& o)
			:m_colnames(o.m_colnames),m_rows(o.m_rows){}

		const std::vector<std::string>& colnames() const	{return m_colnames;}
		const std::vector<Row>& rows() const			{return m_rows;}
		std::size_t size() const				{return m_rows.size();}
		std::vector<Row>::const_iterator begin() const		{return m_rows.begin();}
		std::vector<Row>::const_iterator end() const		{return m_rows.end();}

	private:
		std::vector<std::string> m_colnames;
		std::vector<Row> m_rows;
	};

	/// \brief Execute a single statement with result
	/// \return true if successful, otherwise false (use getLastError to get details)
	bool executeStatement( Result& result, const std::string& stm, const std::vector<types::Variant>& params=std::vector<types::Variant>());
	/// \brief Execute a single statement without result
	/// \return true if successful, otherwise false (use getLastError to get details)
	bool executeStatement( const std::string& stm, const std::vector<types::Variant>& params=std::vector<types::Variant>());

	/// \brief Get the lower lever database specific execution statemachine of the transaction
	TransactionExecStatemachine* execStatemachine()			{return m_stm.get();}
	/// \brief Get the last error occurred
	const DatabaseError* getLastError() const			{return &m_lastError;}

private:
	Transaction( const Transaction&){}	//... non copyable

private:
	std::string m_name;
	TransactionExecStatemachineR m_stm;
	DatabaseError m_lastError;
};


typedef boost::shared_ptr<Transaction> TransactionR;

}} // namespace _Wolframe::db

#endif // _TRANSACTION_HPP_INCLUDED

