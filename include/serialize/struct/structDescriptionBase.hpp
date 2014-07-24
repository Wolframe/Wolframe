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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
/// \file serialize/struct/structDescriptionBase.hpp
/// \brief Defines the non intrusive base class of serialization/deserialization of objects interfaced as TypedInputFilter/TypedOutputFilter.
#ifndef _Wolframe_SERIALIZE_STRUCT_DESCRIPTION_BASE_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_DESCRIPTION_BASE_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "filter/bufferingfilter.hpp"
#include "types/variant.hpp"
#include "serialize/mapContext.hpp"
#include "serialize/struct/parseStack.hpp"
#include "serialize/struct/serializeStack.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include <limits>

namespace _Wolframe {
namespace serialize {

/// \class StructDescriptionBase
/// \brief Base class for structure description used for introspection in serialization/deserialization
class StructDescriptionBase
{
public:
	/// \enum ElementType
	/// \brief Type of a structure element
	enum ElementType
	{
		Atomic,		///< Atomic element type
		Struct,		///< Structure with fixed number of named elements
		Vector		///< Array of structures or atomic values
	};

	typedef std::vector<std::pair<std::string,StructDescriptionBase> > Map;
	typedef bool (*Parse)( langbind::TypedInputFilter& flt, Context& ctx, ParseStateStack& stk);
	typedef bool (*Fetch)( Context& ctx, SerializeStateStack& stk);
	typedef bool (*Constructor)( void* obj);
	typedef void (*Destructor)( void* obj);

	/// \brief Non intrusive parser call function of this sructure
	Parse parse() const		{return m_parse;}
	/// \brief Non intrusive serializer call (fetch the next element) function of this sructure
	Fetch fetch() const		{return m_fetch;}

	/// \enum ElementRequirement
	/// \brief Constraints definition of a structure element
	enum ElementRequirement
	{
		NoRequirement,		///< no explicit condition
		Mandatory,		///< element is always mandatory, even if structure is filled and validated in "relaxed" mode
		Optional		///< element is always optional, even if structure is filled and validated in "strict" mode
	};
	/// \brief Constructor
	StructDescriptionBase( Constructor c, Destructor d, const char* tn, std::size_t os, std::size_t sz, ElementType t, Parse pa, Fetch pr, ElementRequirement req_);
	/// \brief Constructor
	StructDescriptionBase( const char* tn, std::size_t os, std::size_t sz, ElementType t, Parse pa, Fetch pr, ElementRequirement req_);
	/// \brief Copy constructor
	StructDescriptionBase( const StructDescriptionBase& o);
	/// \brief Default constructor
	StructDescriptionBase();

	/// \brief Call of parser of the serialization passed to fill an object and validate the serialization sequence
	bool parse( void* obj, langbind::TypedInputFilter& in, Context& ctx, ParseStateStack& stk) const;

	/// \brief Initializes an atomic element in a structure
	/// \remark Not very efficient implementation
	bool setAtomicValue( void* obj, std::size_t idx, const std::string& value) const;

	/// \brief Initialize the structure referenced with 'obj'
	bool init( void* obj) const
	{
		return (m_constructor)?m_constructor( obj):true;
	}

	/// \brief Destroy the structure referenced with 'obj'
	void done( void* obj) const
	{
		if (m_destructor) m_destructor( obj);
	}

	/// \brief Get the allocation size of this structure in bytes
	std::size_t size() const
	{
		return m_size;
	}

	/// \brief Get the offset of this structure inside the holding parent structure
	std::size_t ofs() const
	{
		return m_ofs;
	}

	/// \brief Get the element kind (type of access) of this structure or atomic element
	ElementType type() const
	{
		return m_type;
	}

	/// \brief Find an element in this structure by name
	Map::const_iterator find( const std::string& name) const;
	/// \brief Case insensitive find an element in this structure by name
	Map::const_iterator find_cis( const std::string& name) const;
	/// \brief Return the element names of this structure as string for error messages
	std::string names( const char* sep) const;

	/// \brief Get the start iterator on the elements of this structure
	Map::const_iterator begin() const {return m_elem.begin();}
	/// \brief Get the end iterator on the elements of this structure
	Map::const_iterator end() const {return m_elem.end();}

	/// \brief Define an element of this structure
	/// \remark Does not check for duplicate definitions
	void define( const std::string& name, const StructDescriptionBase& dd)
	{
		m_elem.push_back( Map::value_type(name,dd));
	}

	/// \brief Get the number of attributes of a struct
	/// \remark returns 0 this is not of type (ContentType) Struct
	/// \return the number of attributes or 0
	std::size_t nof_attributes() const
	{
		return m_nof_attributes;
	}

	/// \brief Define the number of attributes of a struct
	/// \return the number of attributes or 0
	/// \remark returns 0 this is not of type Struct
	void defineEndOfAttributes()
	{
		m_nof_attributes = m_elem.size();
	}

	/// \brief Get the number of elements in the structure or array
	/// \return the number of elements or 0 in case of an atomic value
	std::size_t nof_elements() const
	{
		return m_elem.size();
	}

	/// \brief Get the type name of this structure
	const char* typeName() const
	{
		return m_typename;
	}

	/// \brief Find out if the element in the structure is mandatory
	/// \return true, if yes
	bool mandatory() const				{return m_requirement == Mandatory;}
	/// \brief Find out if the element in the structure is optional
	/// \return true, if yes
	bool optional() const				{return m_requirement == Optional;}
	/// \brief Set element occurrence requirement
	void requirement( ElementRequirement requirement_){m_requirement = requirement_;}

protected:
	StructDescriptionBase& last()
	{
		if (m_elem.empty()) throw std::logic_error( "access of last element in empty structure");
		return m_elem.back().second;
	}

private:
	Constructor m_constructor;
	Destructor m_destructor;
	const char* m_typename;
	std::size_t m_ofs;
	std::size_t m_size;
	std::size_t m_nof_attributes;
	Map m_elem;
	ElementType m_type;
	Parse m_parse;
	Fetch m_fetch;
	ElementRequirement m_requirement;
};

/// \class EmptyStruct
/// \brief Class representing an empty structure
struct EmptyStruct
{
	static const serialize::StructDescriptionBase* getStructDescription();
};

}}//namespace
#endif

