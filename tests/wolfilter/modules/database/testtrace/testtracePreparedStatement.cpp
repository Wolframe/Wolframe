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
///\brief Fake implementation to "process" database commands for testing
///\file modules/database/testtrace/testtracePreparedStatement.cpp
#include "testtracePreparedStatement.hpp"
#include "utils/miscUtils.hpp"
#include <iostream>

using namespace _Wolframe;
using namespace _Wolframe::db;

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

bool FakeResult::next()
{
	if (m_itr != m_data.end()) ++m_itr;
	return (m_itr != m_data.end());
}

unsigned int FakeResult::nofColumns()
{
	return m_cols.size();
}

const char* FakeResult::columnName( std::size_t idx)
{
	return (idx == 0 || idx > m_cols.size())?0:m_cols[idx-1].c_str();
}

const char* FakeResult::get( std::size_t idx)
{
	return (m_itr == m_data.end() || idx == 0 || idx > m_itr->size())?0:(*m_itr)[idx-1].c_str();
}


bool PreparedStatementHandler_testtrace::begin()
{
	m_out << "begin();" << std::endl;
	return true;
}

bool PreparedStatementHandler_testtrace::commit()
{
	m_out << "commit();" << std::endl;
	return true;
}

bool PreparedStatementHandler_testtrace::rollback()
{
	m_out << "rollback();" << std::endl;
	return true;
}

bool PreparedStatementHandler_testtrace::start( const std::string& stmname)
{
	m_out << "start( '" << stmname << "' );" << std::endl;
	return true;
}

bool PreparedStatementHandler_testtrace::bind( std::size_t idx, const char* value)
{
	if (value)
	{
		m_out << "bind( " << idx << ", '" << value << "' );" << std::endl;
	}
	else
	{
		m_out << "bind( " << idx << ", NULL );" << std::endl;
	}
	return true;
}

bool PreparedStatementHandler_testtrace::execute()
{
	++m_resultidx;
	m_out << "execute();" << std::endl;
	return true;
}

std::size_t PreparedStatementHandler_testtrace::nofColumns()
{
	if (m_resultidx == 0 || m_resultidx > m_result.size()) return 0;
	std::size_t rt = m_result[m_resultidx].nofColumns();
	m_out << "nofColumns(); returns " << rt << std::endl;
	return rt;
}

const char* PreparedStatementHandler_testtrace::columnName( std::size_t idx)
{
	if (m_resultidx == 0 || m_resultidx > m_result.size()) return 0;
	const char* rt = m_result[m_resultidx].columnName( idx);
	m_out << "columnName( " << idx << " ); returns " << (rt?rt:"NULL") << std::endl;
	return rt;
}

const char* PreparedStatementHandler_testtrace::getLastError()
{
	m_out << "getLastError(); returns 0" << std::endl;
	return 0;
}

const char* PreparedStatementHandler_testtrace::get( std::size_t idx)
{
	if (m_resultidx == 0 || m_resultidx > m_result.size()) return 0;
	const char* rt = m_result[m_resultidx].get( idx);
	m_out << "get( " << idx << " ); returns " << (rt?rt:"NULL") << std::endl;
	return rt;
}

bool PreparedStatementHandler_testtrace::next()
{
	bool rt = m_result[m_resultidx].next();
	m_out << "next(); returns " << rt << std::endl;
	return rt;
}

std::string PreparedStatementHandler_testtrace::str() const
{
	return m_out.str();
}



