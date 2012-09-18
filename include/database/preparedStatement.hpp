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
///\brief Interface to process prepared statements by a transaction function execute
///\file database/preparedStatement.hpp
#ifndef _DATABASE_PREPARED_STATEMENT_HPP_INCLUDED
#define _DATABASE_PREPARED_STATEMENT_HPP_INCLUDED
#include "types/countedReference.hpp"
#include <string>
#include <cstdlib>

namespace _Wolframe {
namespace db {

///\class PreparedStatementHandler
///\brief Interface to process prepared statements in a database
class PreparedStatementHandler
{
public:
	PreparedStatementHandler()
		:m_timeout(0)
		,m_retries(0){}

	virtual ~PreparedStatementHandler(){}
	///\brief Begin transaction
	virtual bool begin()=0;
	///\brief Commit current transaction
	virtual bool commit()=0;
	///\brief Rollback current transaction
	virtual bool rollback()=0;
	///\brief Clear object and set initial state
	virtual void clear(){}
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
	///\brief Get the last database error as string
	virtual const char* getLastError()=0;
	///\brief Get a column of the last result
	virtual const char* get( std::size_t idx)=0;
	///\brief Skip to the next row of the last result
	virtual bool next()=0;

public:
	void timeout( unsigned short v)		{m_timeout=v;}
	void retries( unsigned short v)		{m_retries=v;}
	unsigned short timeout() const		{return m_timeout;}
	unsigned short retries() const		{return m_retries;}

private:
	unsigned short m_timeout;
	unsigned short m_retries;
};

typedef types::CountedReference<PreparedStatementHandler> PreparedStatementHandlerR;

typedef PreparedStatementHandlerR (*CreatePreparedStatementHandlerFunc)();


}}//namespace
#endif

