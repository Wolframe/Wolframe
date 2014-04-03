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
///\brief Implementation of a tag map for database transaction functions
///\file transactionfunction/TagTable.cpp
#include "transactionfunction/TagTable.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::tf;

TagTable::TagTable( bool case_sensitive_)
	:m_case_sensitive(case_sensitive_),m_size(0)
{
	m_strings.push_back('\0');
	m_strings.push_back( '\0');
}

TagTable::TagTable( const TagTable& o)
	:m_case_sensitive(o.m_case_sensitive)
	,m_size(o.m_size)
	,m_map(o.m_map)
	,m_strings(o.m_strings)
{}

int TagTable::find( const char* tag, std::size_t tagsize) const
{
	const std::string tagnam( tag, tagsize);
	return find( tagnam);
}

int TagTable::find( const std::string& tagnam) const
{
	std::string key = m_case_sensitive?tagnam:boost::algorithm::to_lower_copy(tagnam);
	std::map< std::string, int>::const_iterator ii = m_map.find( key);
	if (ii == m_map.end()) return 0;
	return ii->second;
}

int TagTable::get( const std::string& tagnam)
{
	int rt;
	std::string key = m_case_sensitive?tagnam:boost::algorithm::to_lower_copy(tagnam);
	std::map< std::string, int>::const_iterator ii = m_map.find( key);
	if (ii == m_map.end())
	{
		rt = m_map[ key] = m_strings.size();
		m_strings.append( tagnam);
		m_strings.push_back( '\0');
	}
	else
	{
		rt = ii->second;
	}
	return rt;
}

int TagTable::get( const char* tag, std::size_t tagsize)
{
	const std::string tagstr( tag, tagsize);
	return get( tagstr);
}

std::map<int,int> TagTable::insert( const TagTable& o)
{
	std::map<int,int> rt;
	std::map< std::string, int>::const_iterator oi = o.m_map.begin(), oe = o.m_map.end();
	for (; oi != oe; ++oi)
	{
		int ref = get( oi->first);
		rt[ oi->second] = ref;
	}
	return rt;
}

