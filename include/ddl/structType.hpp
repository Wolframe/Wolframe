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
#include <utility>
#include <vector>
#include <stdexcept>
#include <cstddef>
#include <boost/lexical_cast.hpp>
#include "ddl/atomicType.hpp"

namespace _Wolframe {
namespace ddl {

///\class AtomicType for DDL language binding
class StructType
{
public:
	typedef std::pair< std::string,StructType> Element;
	typedef std::vector< Element> Map;

	enum ContentType
	{
		Atomic,
		Vector,
		Struct
	};

	ContentType contentType() const
	{
		return m_contentType;
	}

	Map::const_iterator find( const char* name) const
	{
		REQUIRE(Struct);
		for (Map::const_iterator itr = m_elem.begin(); itr!=m_elem.end(); ++itr)
		{
			if (std::strcmp( itr->first.c_str(), name) == 0) return itr;
		}
		return m_elem.end();
	}

	Map::iterator find( const char* name)
	{
		REQUIRE(Struct);
		for (Map::iterator itr = m_elem.begin(); itr!=m_elem.end(); ++itr)
		{
			if (std::strcmp( itr->first.c_str(), name) == 0) return itr;
		}
		return m_elem.end();
	}

	Map::const_iterator begin() const
	{
		return (m_contentType == Vector)?(m_elem.begin()+1):m_elem.begin();
	}

	Map::const_iterator end() const
	{
		return m_elem.end();
	}

	AtomicType& value()
	{
		REQUIRE(Atomic);
		return m_value;
	}

	const AtomicType& value() const
	{
		REQUIRE(Atomic);
		return m_value;
	}

	void defineContent( const char* name, const StructType& dd)
	{
		REQUIRE(Struct);
		m_elem.push_back( Element( name, dd));
	}

	void defineAttribute( const char* name, const StructType& dd)
	{
		REQUIRE(Struct);
		dd.REQUIRE(Atomic);
		m_elem.insert( m_elem.begin()+m_nof_attributes, Element( name, dd));
		m_nof_attributes += 1;
	}

	void defineAsVector( const StructType& prototype)
	{
		if (m_contentType == Vector) throw std::logic_error( "prototype of vector defined");
		if (m_contentType == Atomic) throw std::logic_error( "defined as atomic");
		if (m_elem.size()) throw std::logic_error( "defined as structure");
		m_contentType = Vector;
		m_elem.push_back( Element( std::string(), prototype));
	}

	void push()
	{
		REQUIRE(Vector);
		m_elem.push_back( m_elem[0]);
	}

	StructType& back()
	{
		REQUIRE(Vector);
		return m_elem.back().second;
	}

	const StructType& back() const
	{
		REQUIRE(Vector);
		return m_elem.back().second;
	}

	std::size_t nof_attributes() const
	{
		return m_nof_attributes;
	}

	StructType()
		:m_contentType( Struct),m_nof_attributes(0){}
	StructType( const StructType& o)
		:m_contentType(o.m_contentType),m_value(o.m_value),m_elem(o.m_elem),m_nof_attributes(o.m_nof_attributes){}
	StructType( const AtomicType& a)
		:m_contentType( Atomic),m_value(a),m_nof_attributes(0){}

private:
	void REQUIRE( ContentType t) const
	{
		if (m_contentType != t)
		{
			switch(m_contentType)
			{
				case Vector: throw std::logic_error( "not defined as vector");
				case Struct: throw std::logic_error( "not defined as structure");
				case Atomic: throw std::logic_error( "not defined as atomic");
			}
		}
	}

	ContentType m_contentType;
	AtomicType m_value;
	Map m_elem;
	std::size_t m_nof_attributes;
};

}}//namespace
#endif

