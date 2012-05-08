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
///\file serialize/struct/luamapDescription.hpp
///\brief Defines the bricks for the SDK to describe the lua table serializatiom in a readable way.
#ifndef _Wolframe_SERIALIZE_STRUCT_LUAMAP_DESCRIPTION_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_LUAMAP_DESCRIPTION_HPP_INCLUDED
#include "serialize/struct/luamapBase.hpp"
#include "serialize/struct/luamapTraits.hpp"
#include "serialize/struct/luamapParse.hpp"
#include "serialize/struct/luamapPrint.hpp"
#include "logger-v1.hpp"
#include <typeinfo>
#include <exception>

namespace _Wolframe {
namespace serialize {

///\class LuamapDescription
///\brief Intrusive description of a filter/luatable serialization
///\tparam Structure structure that is represented by this description
template <class Structure>
struct LuamapDescription :public LuamapDescriptionBase
{
	///\brief Constructor
	LuamapDescription()
		:LuamapDescriptionBase( &constructor, &destructor, getTypename<Structure>(), 0, sizeof(Structure), &LuamapIntrusiveParser<Structure>::parse, &LuamapIntrusivePrinter<Structure>::print){}

	///\brief Operator to build the structure description element by element
	///\tparam Element element type
	///\param[in] name name of the element
	///\param[in] eptr pointer to member of the element
	template <typename Element>
	LuamapDescription& operator()( const char* name, Element Structure::*eptr)
	{
		// :73:15: error: variable ‘typ’ set but not used [-Werror=unused-but-set-variable]
		// removed 'typ', again not sure here..
		try
		{
			(void)typeid(Element).name();
		}
		catch (std::bad_typeid)
		{}
		LuamapDescriptionBase::Parse parse_ = &LuamapIntrusiveParser<Element>::parse;
		LuamapDescriptionBase::Print print_ = &LuamapIntrusivePrinter<Element>::print;

		std::size_t pp = (std::size_t)&(((Structure*)0)->*eptr);
		LuamapDescriptionBase e( getTypename<Element>(), pp, sizeof(Element), parse_, print_);
		if (find( name) != end())
		{
			LOG_ERROR << "duplicate definition of " << name << " in structure";
		}
		define( name, e);
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
