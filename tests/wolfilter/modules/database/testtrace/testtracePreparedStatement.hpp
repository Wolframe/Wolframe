/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
///\brief Interface to process database commands with fake implementation for testing
///\file modules/database/testtrace/testtracePreparedStatement.hpp
#ifndef _DATABASE_PREPARED_STATEMENT_TESTTRACE_HPP_INCLUDED
#define _DATABASE_PREPARED_STATEMENT_TESTTRACE_HPP_INCLUDED
#include "database/preparedStatement.hpp"
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cstdlib>

namespace _Wolframe {
namespace db {

struct FakeResult
{
public:
	FakeResult( const std::string& str);
	FakeResult( const FakeResult& o)
		:m_data(o.m_data)
		,m_itr(o.m_itr)
		,m_cols(o.m_cols){}

	bool next();
	unsigned int nofColumns();
	const char* columnName( std::size_t idx);
	const char* get( std::size_t idx);

private:
	typedef std::vector<std::string> Row;
	std::vector<Row> m_data;
	std::vector<Row>::const_iterator m_itr;
	std::vector<std::string> m_cols;
};

///\brief Forward declaration
class TesttraceDatabase;

///\class PreparedStatementHandler_testrace
///\brief Handler to process prepared statements as fake implementation for testing
class PreparedStatementHandler_testtrace :public PreparedStatementHandler
{
public:
	///\brief Constructor
	PreparedStatementHandler_testtrace( const std::string& testname_, const std::vector<std::string>& results_, TesttraceDatabase* db_)
		:m_db(db_)
		,m_testname(testname_)
		,m_resultidx(0)
	{
		std::copy( results_.begin(), results_.end(), std::back_inserter(m_result));
	}

	///\brief Destructor
	virtual ~PreparedStatementHandler_testtrace();

	///\brief Begin transaction
	virtual bool begin();
	///\brief Commit current transaction
	virtual bool commit();
	///\brief Rollback current transaction
	virtual bool rollback();
	///\brief Clear object and set initial state
	virtual void clear();
	///\brief Start new command statement
	virtual bool start( const std::string& stmname);
	///\brief Bind parameter value on current command statement
	virtual bool bind( std::size_t idx, const char* value);
	///\brief Execute instance of current statement
	virtual bool execute();
	///\brief Get the number of columns of the last result
	virtual std::size_t nofColumns();
	///\brief Get a column title of the last result
	virtual const char* columnName( std::size_t idx);
	///\brief Get the last database error as string
	virtual const char* getLastError();
	///\brief Get a column of the last result
	virtual const char* get( std::size_t idx);
	///\brief Skip to the next row of the last result
	virtual bool next();

	///\brief Get the dump of all traces
	std::string str() const;

private:
	enum State
	{
		Init,
		Transaction,
		Prepared,
		Executed,
		Error
	};
	static const char* stateName( State i)
	{
		const char* ar[] = {"Init","Transaction","Prepared","Executed","Error"};
		return ar[ (int)i];
	}

private:
	TesttraceDatabase* m_db;
	State m_state;
	std::vector<FakeResult> m_result;
	std::string m_testname;
	std::ostringstream m_out;
	std::size_t m_resultidx;
};

}}//namespace
#endif

