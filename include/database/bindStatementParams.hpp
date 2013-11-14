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
///\brief Interface to substitute parameters in embedded SQL statements
///\file database/bindStatementParams.hpp
#ifndef _DATABASE_BIND_STATEMENT_PARAMETER_STATEMENT_HPP_INCLUDED
#define _DATABASE_BIND_STATEMENT_PARAMETER_STATEMENT_HPP_INCLUDED
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <cstdlib>

namespace _Wolframe {
namespace db {

class Statement
{
public:
	Statement()
		:m_maxparam(0){}
	Statement( const Statement& o)
		:m_data(o.m_data)
		,m_maxparam(o.m_maxparam)
		,m_bind(o.m_bind){}
	explicit Statement( const std::string& stmstr);

	void clear();
	void init( const std::string& stmstr);

	///\remark Does no escaping of parameter because this is dependent on the database !
	void bind( unsigned int idx, const std::string& arg);

	std::string expanded() const;
	const std::string& string() const
	{
		return m_string;
	}

	unsigned int maxparam() const
	{
		return m_maxparam;
	}

private:
	std::string m_string;
	typedef std::pair<unsigned int,std::string> Element;
	std::vector<Element> m_data;
	unsigned int m_maxparam;
	std::map<unsigned int,std::string> m_bind;
};


}}//namespace
#endif

