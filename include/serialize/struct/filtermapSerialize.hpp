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
///\file serialize/struct/filtermapSerialize.hpp
///\brief Defines the intrusive implementation of the serialization of structures
#ifndef _Wolframe_SERLIALIZE_STRUCT_FILTERMAP_SERIALIZE_HPP_INCLUDED
#define _Wolframe_SERLIALIZE_STRUCT_FILTERMAP_SERIALIZE_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "serialize/struct/filtermapTraits.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include "serialize/struct/filtermapSerializeStack.hpp"
#include "serialize/struct/filtermapPrintValue.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

namespace _Wolframe {
namespace serialize {

// forward declaration
template <typename T>
struct FiltermapIntrusiveSerializer
{
	static bool fetch( Context& ctx, FiltermapSerializeStateStack& stk);
};


static bool fetchCloseTag( Context& ctx, FiltermapSerializeStateStack& stk)
{
	ctx.setElem(langbind::FilterBase::CloseTag);
	stk.pop_back();
	return true;
}

static bool fetchOpenTag( Context& ctx, FiltermapSerializeStateStack& stk)
{
	ctx.setElem(
		langbind::FilterBase::OpenTag,
		langbind::TypedFilterBase::Element( stk.back().name()));
	stk.pop_back();
	return true;
}

template <typename T>
static bool fetchObject_( const traits::struct_&, Context& ctx, FiltermapSerializeStateStack& stk)
{
	bool rt = false;
	const void* obj = stk.back().value();

	static const FiltermapDescriptionBase* descr = T::getFiltermapDescription();
	std::size_t idx = stk.back().state();
	if (idx < descr->nof_elements())
	{
		FiltermapDescriptionBase::Map::const_iterator itr = descr->begin() + idx;

		if (idx < descr->nof_attributes())
		{
			if (itr->second.type() != FiltermapDescriptionBase::Atomic)
			{
				throw SerializationErrorException( "atomic value expected for attribute", StructSerializer::getElementPath( stk));
			}
			ctx.setElem(
				langbind::FilterBase::Attribute,
				langbind::TypedFilterBase::Element( itr->first.c_str(), itr->first.size()));
			rt = true;
			stk.back().state( idx+1);
			stk.push_back( FiltermapSerializeState( itr->first.c_str(), itr->second.fetch(), (char*)obj + itr->second.ofs()));
		}
		else
		{
			ctx.setElem(
				langbind::FilterBase::OpenTag,
				langbind::TypedFilterBase::Element( itr->first.c_str(), itr->first.size()));
			rt = true;
			stk.back().state( idx+1);
			stk.push_back( FiltermapSerializeState( 0, &fetchCloseTag, itr->first.c_str()));
			stk.push_back( FiltermapSerializeState( itr->first.c_str(), itr->second.fetch(), (char*)obj + itr->second.ofs()));
		}
	}
	else
	{
		stk.pop_back();
		if (stk.size() == 0)
		{
			ctx.setElem( langbind::FilterBase::CloseTag, langbind::TypedFilterBase::Element());
			rt = true;
		}
	}
	return rt;
}

template <typename T>
static bool fetchObject_( const traits::atomic_&, Context& ctx, FiltermapSerializeStateStack& stk)
{
	Context::ElementBuffer elem;
	elem.m_type = langbind::FilterBase::Value;
	if (!traits::printValue( *(T*)stk.back().value(), elem.m_value))
	{
		throw SerializationErrorException( "atomic value conversion error", StructSerializer::getElementPath( stk));
	}
	ctx.setElem( elem);
	stk.pop_back();
	return true;
}

template <typename T>
static bool fetchObject_( const traits::vector_&, Context& ctx, FiltermapSerializeStateStack& stk)
{
	bool rt = false;
	std::vector<typename T::value_type>* obj = (T*)stk.back().value();
	std::size_t idx = stk.back().state();
	if (idx >= obj->size())
	{
		stk.pop_back();
		return false;
	}
	if (ctx.flag( Context::SerializeWithIndices))
	{
		ctx.setElem( langbind::FilterBase::OpenTag, langbind::TypedFilterBase::Element( (unsigned int)idx+1));
		stk.back().state( idx+1);
		typename T::value_type* ve = &(*obj)[ idx];
		stk.push_back( FiltermapSerializeState( 0, &fetchCloseTag, 0));
		stk.push_back( FiltermapSerializeState( stk.back().name(), &FiltermapIntrusiveSerializer<typename T::value_type>::fetch, ve));
	}
	else
	{
		const char* tagname = (const char*)stk.at( stk.size()-2).value();
		if (idx >= 1)
		{
			ctx.setElem( langbind::FilterBase::CloseTag);
			rt = true;
		}
		stk.back().state( idx+1);
		typename T::value_type* ve = &(*obj)[ idx];
		stk.push_back( FiltermapSerializeState( stk.back().name(), &FiltermapIntrusiveSerializer<typename T::value_type>::fetch, ve));
		if (idx >= 1)
		{
			stk.push_back( FiltermapSerializeState( tagname, &fetchOpenTag, tagname));
		}
	}
	return rt;
}

template <typename T>
bool FiltermapIntrusiveSerializer<T>::fetch( Context& ctx, FiltermapSerializeStateStack& stk)
{
	static T* t = 0;
	return fetchObject_<T>( traits::getFiltermapCategory(*t), ctx, stk);
}

}}//namespace
#endif

