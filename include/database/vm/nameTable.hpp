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
//\file database/vm/nameTable.hpp
//\brief Defines a set of names assigned to ascending indices
#ifndef _DATABASE_VIRTUAL_MACHINE_NAME_TABLE_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_NAME_TABLE_HPP_INCLUDED
#include "database/vm/instructionSet.hpp"
#include "types/keymap.hpp"
#include <map>
#include <vector>
#include <string>
#include <cstdlib>

namespace _Wolframe {
namespace db {
namespace vm {

class NameTable
{
public:
	typedef InstructionSet::ArgumentIndex Index;

public:
	NameTable(){}
	NameTable( const NameTable& o)
		:m_namemap(o.m_namemap)
		,m_namear(o.m_namear){}

	const std::string& getName( const Index& idx) const
	{
		if (idx > m_namear.size()) throw std::runtime_error("name reference out of range");
		if (idx == 0) throw std::runtime_error( "name reference is NULL");
		return m_namear.at(idx-1);
	}

	Index getIndex( const std::string& nam) const
	{
		NameMap::const_iterator si = m_namemap.find( nam);
		if (si == m_namemap.end()) return 0;
		if (si->second == 0) return 0; 
		return m_namear.at( si->second);
	}

	Index define( const std::string& nam)
	{
		m_namemap.insert( nam, m_namear.size()+1);
		m_namear.push_back( nam);
		return m_namear.size();
	}

	Index get( const std::string& nam)
	{
		NameMap::const_iterator si = m_namemap.find( nam);
		if (si == m_namemap.end())
		{
			return define( nam);
		}
		return si->second;
	}

private:
	typedef types::keymap<Index> NameMap;
	NameMap m_namemap;
	std::vector<std::string> m_namear;
};

}}}//namespace
#endif

