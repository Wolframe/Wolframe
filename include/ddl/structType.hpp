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
///\file ddl/structType.hpp
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

///\class StructType
///\brief generic type for DDL language binding
class StructType
{
public:
	typedef std::pair< std::string,StructType> Element;
	typedef std::vector< Element> Map;

	enum ContentType
	{
		Atomic,		//< atomic type
		Vector,		//< vector of StructType elements (that are Atomic or Struct)
		Struct		//< structure type
	};

	ContentType contentType() const
	{
		return m_contentType;
	}

	StructType()
		:m_contentType( Struct),m_nof_attributes(0){}
	StructType( const StructType& o)
		:m_contentType(o.m_contentType),m_value(o.m_value),m_elem(o.m_elem),m_nof_attributes(o.m_nof_attributes){}
	StructType( const AtomicType& a)
		:m_contentType( Atomic),m_value(a),m_nof_attributes(0){}

	StructType& operator= ( const StructType& o);


	Map::const_iterator find( const char* name) const;

	Map::iterator find( const char* name);

	Map::const_iterator begin() const;

	Map::const_iterator end() const;

	AtomicType& value();

	const AtomicType& value() const;

	void defineContent( const char* name, const StructType& dd);
	void defineContent( const std::string& name, const StructType& dd);

	void defineAttribute( const char* name, const StructType& dd);
	void defineAttribute( const std::string& name, const StructType& dd);

	void defineAsVector( const StructType& prototype);

	void push();

	StructType& back();

	const StructType& back() const;

	std::size_t nof_attributes() const;

private:
	void REQUIRE( ContentType t) const;

	ContentType m_contentType;
	AtomicType m_value;
	Map m_elem;
	std::size_t m_nof_attributes;
};

}}//namespace
#endif

