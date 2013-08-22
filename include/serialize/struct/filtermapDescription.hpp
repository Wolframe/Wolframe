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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file serialize/struct/filtermapDescription.hpp
///\brief Defines the bricks for the SDK to describe serialization/deserialization of objects in Wolframe.
///\remark This module uses intrusive building blocks to build the serialization/deserialization of the objects interfaced as TypedInputFilter/TypedOutputFilter.
#ifndef _Wolframe_SERIALIZE_STRUCT_FILTERMAP_DESCRIPTION_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_FILTERMAP_DESCRIPTION_HPP_INCLUDED
#include "serialize/struct/filtermapBase.hpp"
#include "serialize/struct/filtermapTraits.hpp"
#include "serialize/struct/filtermapParse.hpp"
#include "serialize/struct/filtermapSerialize.hpp"
#include "serialize/struct/filtermapProperty.hpp"
#include <exception>
#include <stdexcept>
#include <sstream>

namespace _Wolframe {
namespace serialize {

///\class StructDescription
///\brief Intrusive description of a filter/form map
///\tparam Structure structure that is represented by this description
template <class Structure>
struct StructDescription :public StructDescriptionBase
{
	///\brief Constructor
	StructDescription()
		:StructDescriptionBase( &constructor, &destructor, getTypename<Structure>(), 0, sizeof(Structure), FiltermapIntrusiveProperty<Structure>::type(), &FiltermapIntrusiveParser<Structure>::parse, &FiltermapIntrusiveSerializer<Structure>::fetch, NoRequirement){}

	///\brief Operator to build the structure description element by element
	///\tparam Element element type
	///\param[in] tag name of the element
	///\param[in] eptr pointer to member of the element
	template <typename Element>
	StructDescription& operator()( const char* tag, Element Structure::*eptr)
	{
		StructDescriptionBase::Parse parse_ = &FiltermapIntrusiveParser<Element>::parse;
		StructDescriptionBase::Fetch fetch_ = &FiltermapIntrusiveSerializer<Element>::fetch;
		StructDescriptionBase::ElementType type_ = FiltermapIntrusiveProperty<Element>::type();
		std::size_t pp = (std::size_t)&(((Structure*)0)->*eptr);
		ElementRequirement requirement_ = NoRequirement;
		if (tag[0]=='?') requirement_ = Optional;
		if (tag[0]=='!') requirement_ = Mandatory;
		const char* name = (requirement_ != NoRequirement)?(tag+1):tag;

		StructDescriptionBase e( getTypename<Element>(), pp, sizeof(Element), type_, parse_, fetch_, requirement_);
		if (find( name) != end())
		{
			std::ostringstream err;
			err << "duplicate definition of " << name << " in structure";
			throw std::runtime_error( err.str().c_str());
		}
		define( name, e);
		return *this;
	}

	StructDescription& operator--(int)
	{
		defineEndOfAttributes();
		return *this;
	}

	static bool constructor( void* obj)
	{
		///\brief create operator new for Structure on an already allocated chunk to call it's constructor
		struct StructureShell :public Structure
		{
			void* operator new( std::size_t num_bytes, void* obj) throw (std::bad_alloc)
			{
				if (sizeof(StructureShell) != num_bytes) throw std::bad_alloc();
				return obj;
			}
		};
		try
		{
			new (obj) StructureShell();
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}

	static void destructor( void* obj)
	{
		((Structure*)obj)->~Structure();
	}

	template <class Element>
	static const char* getTypename()
	{
		const char* typ = 0;
		try
		{
			typ = typeid(Element).name();
		}
		catch (std::bad_typeid)
		{}
		return typ;
	}
};

}}// end namespace
#endif

