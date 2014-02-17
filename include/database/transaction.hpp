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
//\file database/transaction.hpp
//\brief Interface of a database transaction

#ifndef _TRANSACTION_HPP_INCLUDED
#define _TRANSACTION_HPP_INCLUDED
#include "database/transactionInput.hpp"
#include "database/transactionOutput.hpp"
#include "types/countedReference.hpp"
#include "types/variant.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace db {

//\brief Transaction interface
struct Transaction
{
	//\brief Destructor
	virtual ~Transaction(){}
	//\brief Configured ID of the underlaying database
	virtual const std::string& databaseID() const=0;

	//\brief Begin of a new transaction
	virtual void begin()=0;
	//\brief Commit of the running transaction
	virtual void commit()=0;
	//\brief Rollback of the running transaction
	virtual void rollback()=0;
	//\brief Close of the committed or rolled back transaction
	virtual void close()=0;

	//\brief Execute a transaction
	virtual void execute( const TransactionInput& input, TransactionOutput& output)=0;

	//\class Result
	//\brief Result of a single statement execute call: executeStatement( const std::string&, const std::vector<types::Variant>&);
	class Result
	{
	public:
		typedef std::vector<types::Variant> Row;

		Result(){}
		Result( const std::vector<std::string>& colnames_, const std::vector<Row>& rows_)
			:m_colnames(colnames_),m_rows(rows_){}
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

	//\brief Execute a single statement
	Result executeStatement( const std::string& stm, const std::vector<types::Variant>& params=std::vector<types::Variant>());
};


typedef types::CountedReference<Transaction> TransactionR;

}} // namespace _Wolframe::db

#endif // _TRANSACTION_HPP_INCLUDED
