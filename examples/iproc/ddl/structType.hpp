/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file ddl/atomicType.hpp
///\brief Defines an an intrusive structure type as basic mapping type for DDLs

#ifndef _Wolframe_DDL_STRUCTTYPE_HPP_INCLUDED
#define _Wolframe_DDL_STRUCTTYPE_HPP_INCLUDED
#include <string>
#include <vector>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include "ddl/atomicType.hpp"

namespace _Wolframe {
namespace ddl {

///\class AtomicType for DDL language binding
class StructType
{
public:
	typedef std::vector<std::string,StructType> Map;

	bool isAtomic() const
	{
		return (m_elem.size()==0);
	}

	Map::const_iterator find( const char* name) const
	{
		for (Map::const_iterator itr = m_elem.begin(); itr!=m_elem.end(); ++itr)
		{
			if (std::strcmp( itr->first.c_str(), name) == 0) return itr;
		}
		return m_elem.end();
	}

	Map::const_iterator begin() const {return m_elem.begin();}
	Map::const_iterator end() const {return m_elem.end();}

	void define( const char* name, const StructType& dd)
	{
		m_elem.push_back( std::pair<std::string,StructType>(name,dd));
	}

	StructType( const StructType& o)
		:m_value(o.m_value),m_elem(o.m_elem){}
	StructType( const AtomicType& a)
		:m_value(a.value()){}

private:
	AtomicType m_value;
	Map m_elem;
};

}}//namespace
#endif

