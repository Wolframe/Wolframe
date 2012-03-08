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
///\file ddl_structType.cpp
///\brief implementation of the form DDL generic type
#include "ddl/structType.hpp"

using namespace _Wolframe;
using namespace ddl;

void StructType::REQUIRE( ContentType t) const
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

StructType::Map::const_iterator StructType::find( const char* name) const
{
	REQUIRE(Struct);
	for (Map::const_iterator itr = m_elem.begin(); itr!=m_elem.end(); ++itr)
	{
		if (std::strcmp( itr->first.c_str(), name) == 0) return itr;
	}
	return m_elem.end();
}

StructType::Map::iterator StructType::find( const char* name)
{
	REQUIRE(Struct);
	for (Map::iterator itr = m_elem.begin(); itr!=m_elem.end(); ++itr)
	{
		if (std::strcmp( itr->first.c_str(), name) == 0) return itr;
	}
	return m_elem.end();
}

StructType::Map::const_iterator StructType::begin() const
{
	return (m_contentType == Vector)?(m_elem.begin()+1):m_elem.begin();
}

StructType::Map::const_iterator StructType::end() const
{
	return m_elem.end();
}

AtomicType& StructType::value()
{
	REQUIRE(Atomic);
	return m_value;
}

const AtomicType& StructType::value() const
{
	REQUIRE(Atomic);
	return m_value;
}

void StructType::defineContent( const char* name, const StructType& dd)
{
	REQUIRE(Struct);
	m_elem.push_back( Element( name, dd));
}

void StructType::defineContent( const std::string& name, const StructType& dd)
{
	defineContent( name.c_str(), dd);
}

void StructType::defineAttribute( const char* name, const StructType& dd)
{
	REQUIRE(Struct);
	dd.REQUIRE(Atomic);
	m_elem.insert( m_elem.begin()+m_nof_attributes, Element( name, dd));
	m_nof_attributes += 1;
}

void StructType::defineAttribute( const std::string& name, const StructType& dd)
{
	defineAttribute( name.c_str(), dd);
}

void StructType::defineAsVector( const StructType& prototype)
{
	if (m_contentType == Vector) throw std::logic_error( "prototype of vector defined");
	if (m_contentType == Atomic) throw std::logic_error( "defined as atomic");
	if (m_elem.size()) throw std::logic_error( "defined as structure");
	m_contentType = Vector;
	m_elem.push_back( Element( std::string(), prototype));
}

void StructType::push()
{
	REQUIRE(Vector);
	m_elem.push_back( m_elem[0]);
}

StructType& StructType::back()
{
	REQUIRE(Vector);
	return m_elem.back().second;
}

const StructType& StructType::back() const
{
	REQUIRE(Vector);
	return m_elem.back().second;
}

std::size_t StructType::nof_attributes() const
{
	return m_nof_attributes;
}

StructType& StructType::operator= ( const StructType& o)
{
	m_contentType = o.m_contentType;
	m_value = o.m_value;
	m_elem = o.m_elem;
	m_nof_attributes = o.m_nof_attributes;
	return *this;
}

