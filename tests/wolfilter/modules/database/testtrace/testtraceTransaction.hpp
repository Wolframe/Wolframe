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
//
//
//

#ifndef _TESTTRACE_TRANSACTION_HPP_INCLUDED
#define _TESTTRACE_TRANSACTION_HPP_INCLUDED
#include "database/transaction.hpp"
#include "testtraceDatabase.hpp"
#include <string>

namespace _Wolframe {
namespace db {

class TesttraceTransaction
	:public Transaction
{
public:
	TesttraceTransaction( const TesttraceDatabase* dbref_, const std::vector<std::string>& result_)
		:m_dbref(dbref_)
		,m_result(result_){}

	TesttraceTransaction( const TesttraceTransaction& o)
		:m_dbref(o.m_dbref)
		,m_result(o.m_result){}

	virtual ~TesttraceTransaction(){}

	virtual const std::string& databaseID() const
	{
		return m_dbref->ID();
	}

	virtual void execute();
	virtual void begin();
	virtual void commit();
	virtual void rollback();

	virtual void putInput( const TransactionInput& input_)		{m_input = input_;}
	virtual const TransactionOutput& getResult() const		{return m_output;}

	virtual void close(){}

private:
	const TesttraceDatabase* m_dbref;
	std::vector<std::string> m_result;
	TransactionInput m_input;
	TransactionOutput m_output;
};

}} // namespace _Wolframe::db

#endif // _TRANSACTION_HPP_INCLUDED
