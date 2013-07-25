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
///\brief Fake implementation to "process" database commands for testing
///\file modules/database/testtrace/testtraceTransaction.cpp
#include "testtraceTransaction.hpp"
#include "database/preparedStatement.hpp"
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


class TransactionHandler :public PreparedStatementHandler
{
public:
	///\brief Constructor
	TransactionHandler( std::ostream* out, const std::vector<std::string>& res)
		:m_res(res)
		,m_out(out)
	{
		m_resitr = m_res.begin();
	}

	TransactionHandler( const TransactionHandler& o)
		:m_res(o.m_res)
		,m_out(o.m_out)
	{
		m_resitr = m_res.begin();
	}

	virtual bool start( const std::string& stmname)
	{
		(*m_out) << "start( '" << stmname << "' );" << std::endl;
		return true;
	}

	virtual bool bind( std::size_t idx, const char* value)
	{
		if (value)
		{
			(*m_out) << "bind( " << idx << ", '" << value << "' );" << std::endl;
		}
		else
		{
			(*m_out) << "bind( " << idx << ", NULL );" << std::endl;
		}
		return true;
	}

	virtual bool execute()
	{
		(*m_out) << "execute();" << std::endl;
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
		(*m_out) << "next(); returns " << (int)rt << std::endl;
		return rt;
	}

	virtual std::size_t nofColumns()
	{
		std::size_t rt = m_fakeres.nofColumns();
		(*m_out) << "nofColumns(); returns " << rt << std::endl;
		return rt;
	}

	virtual const char* columnName( std::size_t idx)
	{
		const char* rt = m_fakeres.columnName( idx);
		(*m_out) << "columnName( " << idx << "); returns " << (rt?rt:"NULL") << std::endl;
		return rt;
	}

	virtual const char* get( std::size_t idx)
	{
		const char* rt = m_fakeres.get( idx);
		(*m_out) << "get( " << idx << " ); returns " << rt << std::endl;
		return rt;
	}

	virtual const db::DatabaseError* getLastError()
	{
		return 0;
	}

private:
	std::vector<std::string> m_res;
	std::vector<std::string>::const_iterator m_resitr;
	std::ostream* m_out;
	FakeResult m_fakeres;
};


static void printTransactionInput( std::ostream& out, const TransactionInput& input)
{
	TransactionInput::cmd_iterator ci = input.begin(), ce = input.end();
	for (; ci != ce; ++ci)
	{
		out << ci->name();
		TransactionInput::arg_iterator ai = ci->begin(), ae = ci->end();
		if (ai != ae) out << " ";

		for (; ai != ae; ++ai)
		{
			switch (ai->type())
			{
				case TransactionInput::Element::ResultColumn:
					out << "#[" << ai->ref() << "]";
					break;
				case TransactionInput::Element::String:
					out << "#" << (ai->value()?ai->value():"NULL");
					break;
			}
		}
		out << std::endl;
	}
	out << std::endl;
}

void TesttraceTransaction::begin()
{
	throw std::runtime_error( "transactional context (begin) not implemented in testtrace database");
}

void TesttraceTransaction::commit()
{
	throw std::runtime_error( "transactional context (commit) not implemented in testtrace database");
}

void TesttraceTransaction::rollback()
{
	throw std::runtime_error( "transactional context (rollback) not implemented in testtrace database");
}

void TesttraceTransaction::execute()
{
	std::ofstream buf( m_dbref->outfilename().c_str());
	printTransactionInput( buf, m_input);

	TransactionHandler stm( &buf, m_result);
	if (!stm.doTransaction( m_input, m_output))
	{
		const db::DatabaseError* err = stm.getLastError();
		if (err)
		{
			throw db::DatabaseErrorException( *err);
		}
		else
		{
			throw std::runtime_error( "unspecified error");
		}
	}
}



