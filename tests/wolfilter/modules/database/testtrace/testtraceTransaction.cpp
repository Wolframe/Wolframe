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
///\brief Fake implementation to "process" database commands for testing
///\file modules/database/testtrace/testtraceTransaction.cpp
#include "testtraceTransaction.hpp"
#include "database/transactionExecStatemachine.hpp"
#include "types/variant.hpp"
#include "utils/stringUtils.hpp"
#include "utils/parseUtils.hpp"
#include <iostream>
#include <fstream>
#include <limits>

using namespace _Wolframe;
using namespace _Wolframe::db;

struct FakeResult
{
public:
	FakeResult()
	{
		m_itr = m_data.begin();
	}
	FakeResult( const std::string& str);
	FakeResult( const FakeResult& o)
		:m_data(o.m_data)
		,m_cols(o.m_cols)
	{
		m_itr = m_data.begin();
	}

	FakeResult& operator=( const FakeResult& o)
	{
		m_data = o.m_data;
		m_cols = o.m_cols;
		m_itr = m_data.begin();
		return *this;
	}

	bool next()
	{
		if (m_itr != m_data.end()) ++m_itr;
		return (m_itr != m_data.end());
	}

	unsigned int nofColumns()
	{
		return m_cols.size();
	}

	const char* columnName( std::size_t idx)
	{
		return (idx == 0 || idx > m_cols.size())?0:m_cols[idx-1].c_str();
	}

	const char* get( std::size_t idx)
	{
		return (m_itr == m_data.end() || idx == 0 || idx > m_itr->size())?0:(*m_itr)[idx-1].c_str();
	}

	unsigned int nofResults()
	{
		return m_data.size();
	}

private:
	typedef std::vector<std::string> Row;
	std::vector<Row> m_data;
	std::vector<Row>::const_iterator m_itr;
	std::vector<std::string> m_cols;
};

FakeResult::FakeResult( const std::string& str)
{
	if (str.empty()) return;
	char rowdelim[2];
	rowdelim[0] = str[0];
	rowdelim[1] = '\0';

	std::vector<std::string> rows;
	utils::splitString( rows, str.begin()+1, str.end(), rowdelim);

	std::vector<std::string>::const_iterator roi=rows.begin(), roe=rows.end();
	if (roi == roe) return;
	std::string::const_iterator il = roi->begin(), el = roi->end();
	std::string colname;

	while (utils::parseNextToken( colname, il, el))
	{
		m_cols.push_back( colname);
	}

	for (++roi; roi != roe; ++roi)
	{
		il = roi->begin(), el = roi->end();
		std::string tok;
		std::vector<std::string> row;

		while (utils::parseNextToken( tok, il, el))
		{
			row.push_back( tok);
		}
		m_data.push_back( row);
	}
	m_itr = m_data.begin();
}


class TransactionHandler
	:public TransactionExecStatemachine
{
public:
	typedef TransactionExecStatemachine Parent;

	///\brief Constructor
	TransactionHandler( const std::string& outfilename, const std::vector<std::string>& res)
		:m_res(res)
		,m_out(outfilename.c_str())
	{
		m_resitr = m_res.begin();
	}

	std::ofstream& out()
	{
		return m_out;
	}

	virtual const std::string& databaseID() const
	{
		static const std::string rt = "TEST";
		return rt;
	}

	virtual bool begin(){return true;}
	virtual bool commit(){return true;}
	virtual bool rollback(){return true;}

	virtual bool start( const std::string& stmname)
	{
		m_out << "start( '" << stmname << "' );" << std::endl;
		return true;
	}

	virtual bool bind( std::size_t idx, const types::VariantConst& value)
	{
		if (value.defined())
		{
			m_out << "bind( " << idx << ", '" << value.tostring() << "' );" << std::endl;
		}
		else
		{
			m_out << "bind( " << idx << ", NULL );" << std::endl;
		}
		return true;
	}

	virtual bool execute()
	{
		m_out << "execute();" << std::endl;
		if (m_resitr == m_res.end())
		{
			m_fakeres = FakeResult();
		}
		else
		{
			m_fakeres = FakeResult( *m_resitr++);
		}
		return true;
	}

	virtual bool hasResult()
	{
		return (m_fakeres.nofResults() != 0);
	}

	virtual bool next()
	{
		bool rt = m_fakeres.next();
		m_out << "next(); returns " << (int)rt << std::endl;
		return rt;
	}

	virtual std::size_t nofColumns()
	{
		std::size_t rt = m_fakeres.nofColumns();
		m_out << "nofColumns(); returns " << rt << std::endl;
		return rt;
	}

	virtual const char* columnName( std::size_t idx)
	{
		const char* rt = m_fakeres.columnName( idx);
		m_out << "columnName( " << idx << "); returns " << (rt?rt:"NULL") << std::endl;
		return rt;
	}

	virtual types::VariantConst get( std::size_t idx)
	{
		const char* rt = m_fakeres.get( idx);
		m_out << "get( " << idx << " ); returns " << rt << std::endl;
		if (rt)
		{
			return types::VariantConst(rt);
		}
		else
		{
			return types::VariantConst(rt);
		}
	}

	virtual const db::DatabaseError* getLastError()
	{
		return 0;
	}

	///\brief Find out if the database is case sensitive or not
	virtual bool isCaseSensitive()	{return false;}

private:
	std::vector<std::string> m_res;
	std::vector<std::string>::const_iterator m_resitr;
	std::ofstream m_out;
	FakeResult m_fakeres;
};


TesttraceTransaction::TesttraceTransaction( const TesttraceDatabase* dbref_, const std::vector<std::string>& result_)
	:Transaction( "TEST", TransactionExecStatemachineR( new TransactionHandler( dbref_->outfilename().c_str(), result_)))
	,m_result(result_)
{}

bool TesttraceTransaction::execute( const VmTransactionInput& input, VmTransactionOutput& output)
{
	TransactionHandler* handler = dynamic_cast<TransactionHandler*>( execStatemachine());
	input.print( handler->out());
	return Transaction::execute( input, output);
}



