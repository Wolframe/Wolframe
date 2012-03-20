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
///\brief Defines a non intrusive structure type for values in the DDLs used for forms

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

	///\enum ContentType
	///\brief What StructType can be
	enum ContentType
	{
		Atomic,		//< atomic type
		Vector,		//< vector of StructType elements (that are Atomic or Struct)
		Struct		//< structure type
	};

	///\brief Get the content type of this
	///\return the content type of this
	ContentType contentType() const
	{
		return m_contentType;
	}

	///\brief Constructor
	StructType()
		:m_contentType( Struct),m_nof_attributes(0){}
	///\brief Copy constructor
	///\param[in] o element to copy
	StructType( const StructType& o)
		:m_contentType(o.m_contentType),m_value(o.m_value),m_elem(o.m_elem),m_nof_attributes(o.m_nof_attributes){}
	///\brief Copy constructor
	///\param[in] a element to copy
	StructType( const AtomicType& a)
		:m_contentType( Atomic),m_value(a),m_nof_attributes(0){}

	///\brief Assignement operator
	///\param[in] o element to copy
	StructType& operator= ( const StructType& o);

	///\brief Find an element of this as a structure
	///\remark [precondition] this must be of type (ContentType) Struct
	///\param[in] name name of the element to find
	Map::const_iterator find( const char* name) const;

	///\brief Find an element of this as a structure
	///\remark [precondition] this must be of type (ContentType) Struct
	///\param[in] name name of the element to find
	Map::const_iterator find( const std::string& name) const;

	///\brief Find an element of this as a structure
	///\remark [precondition] this must be of type (ContentType) Struct
	///\param[in] name name of the element to find
	Map::iterator find( const char* name);

	///\brief Find an element of this as a structure
	///\remark [precondition] this must be of type (ContentType) Struct
	///\param[in] name name of the element to find
	Map::iterator find( const std::string& name);

	///\brief Get the begin marker iterator on the elements of this
	///\remark [precondition] this must not be of type (ContentType) Atomic
	///\return the begin iterator
	Map::const_iterator begin() const;

	///\brief Get the end marker iterator on the elements of this
	///\remark [precondition] this must not be of type (ContentType) Atomic
	///\return the end iterator
	Map::const_iterator end() const;

	///\brief Get the value of this as an atomic type
	///\remark [precondition] this must be of type (ContentType) Atomic
	///\return the value of this
	AtomicType& value();

	///\brief Get the value of this as an atomic type
	///\remark [precondition] this must be of type (ContentType) Atomic
	///\return the value of this
	const AtomicType& value() const;

	///\brief Define a content element of a structure
	///\remark [precondition] this must be of type (ContentType) Struct
	///\param[in] name name of the element defined
	///\param[in] val value of the element defined as member of this
	void defineContent( const char* name, const StructType& val);

	///\brief Define a content element of a structure
	///\remark [precondition] this must be of type (ContentType) Struct
	///\param[in] name name of the element defined
	///\param[in] val value of the element defined as member of this
	void defineContent( const std::string& name, const StructType& val);

	///\brief Define an attribute of a structure
	///\remark [precondition] this must be of type (ContentType) Struct
	///\remark [precondition] val must be of type atomic
	///\param[in] name name of the attribute defined
	///\param[in] val value of the attribute defined as member of this
	void defineAttribute( const char* name, const StructType& val);

	///\brief Define an attribute of a structure
	///\remark [precondition] this must be of type (ContentType) Struct
	///\remark [precondition] val must be of type atomic
	///\param[in] name name of the attribute defined
	///\param[in] val value of the attribute defined as member of this
	void defineAttribute( const std::string& name, const StructType& val);

	///\brief Define this to be a vector of elements (ContentType Vector) of type defined by prototype
	///\remark [precondition] this must not be of type (ContentType) Vector yet.
	///\remark [precondition] prototype must not be of type (ContentType) Vector
	///\param[in] prototype prototype of an empty vector element (new elements are created as copies of prototype)
	void defineAsVector( const StructType& prototype);

	///\brief Add a new element (copy of prototype) to this as a vector of elements (ContentType Vector)
	///\remark [precondition] this must be of type (ContentType) Vector
	void push();

	///\brief Access the topmost element of a vector of elements (ContentType Vector)
	///\remark [precondition] this must be of type (ContentType) Vector
	StructType& back();

	///\brief Access the topmost element of a vector of elements (ContentType Vector)
	///\remark [precondition] this must be of type (ContentType) Vector
	const StructType& back() const;

	///\brief Access the prototype element of a vector of elements (ContentType Vector)
	///\remark [precondition] this must be of type (ContentType) Vector
	StructType& prototype();

	///\brief Access the prototype element of a vector of elements (ContentType Vector)
	///\remark [precondition] this must be of type (ContentType) Vector
	const StructType& prototype() const;

	///\brief Get the number of attributes of a struct
	///\remark returns 0 this is not of type (ContentType) Struct
	///\return the number of attributes or 0
	std::size_t nof_attributes() const;

	///\brief Print content to out
	///\param[out] out output stream to print to
	///\param[in] indent indentiation for items to print
	void print( std::ostream& out, size_t indent=0) const;

private:
	///\brief Assert a type precondition of this. (throws an logic_error exception on failure)
	///\remark Used for checking the preconditions mentioned as remark [precondition]
	void REQUIRE( ContentType t) const;

	ContentType m_contentType;
	AtomicType m_value;
	Map m_elem;
	std::size_t m_nof_attributes;
};

}}//namespace
#endif

