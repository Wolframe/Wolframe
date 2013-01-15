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
///\file serialize/struct/filtermapSerialize.hpp
///\brief Defines the intrusive implementation of the serialization of structures
#ifndef _Wolframe_SERLIALIZE_STRUCT_FILTERMAP_SERIALIZE_HPP_INCLUDED
#define _Wolframe_SERLIALIZE_STRUCT_FILTERMAP_SERIALIZE_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "serialize/struct/filtermapTraits.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include "serialize/struct/filtermapSerializeStack.hpp"
#include <string>
#include <vector>
#include <map>

namespace _Wolframe {
namespace serialize {

// forward declaration
template <typename TYPE>
struct FiltermapIntrusiveSerializer
{
	static bool fetch( Context& ctx, FiltermapSerializeStateStack& stk);
};

bool fetchCloseTag( Context& ctx, FiltermapSerializeStateStack& stk);

bool fetchOpenTag( Context& ctx, FiltermapSerializeStateStack& stk);

bool fetchObjectStruct( const StructDescriptionBase* descr, Context& ctx, FiltermapSerializeStateStack& stk);

typedef bool (*PrintValue)( const void* ptr, langbind::TypedFilterBase::Element& value);

bool fetchObjectAtomic( PrintValue prnt, Context& ctx, FiltermapSerializeStateStack& stk);

typedef bool (*PrintValue)( const void* value, langbind::TypedFilterBase::Element& element);

bool printValue_int( const signed int*, langbind::TypedFilterBase::Element& element);
bool printValue_uint( const unsigned int*, langbind::TypedFilterBase::Element& element);
bool printValue_ulong( const unsigned long*, langbind::TypedFilterBase::Element& element);
bool printValue_long( const signed long*, langbind::TypedFilterBase::Element& element);
bool printValue_short( const signed short*, langbind::TypedFilterBase::Element& element);
bool printValue_ushort( const unsigned short*, langbind::TypedFilterBase::Element& element);
bool printValue_char( const signed char*, langbind::TypedFilterBase::Element& element);
bool printValue_uchar( const unsigned char*, langbind::TypedFilterBase::Element& element);
bool printValue_float( const float*, langbind::TypedFilterBase::Element& element);
bool printValue_double( const double*, langbind::TypedFilterBase::Element& element);
bool printValue_string( const std::string*, langbind::TypedFilterBase::Element& element);

typedef bool (*FetchElement)( Context& ctx, FiltermapSerializeStateStack& stk);

bool fetchObjectVectorElement( FetchElement fetchElement, const void* ve, Context& ctx, FiltermapSerializeStateStack& stk);

namespace {
template <typename TYPE>
static bool printValue_( const void*, langbind::TypedFilterBase::Element&)
{throw std::runtime_error( "unable to serialize atomic value of this type");}
template <> bool printValue_<signed int>( const void* value, langbind::TypedFilterBase::Element& element)
{return printValue_int( (const int*)value, element);}
template <> bool printValue_<unsigned int>( const void* value, langbind::TypedFilterBase::Element& element)
{return printValue_uint( (const unsigned int*)value, element);}
template <> bool printValue_<signed long>( const void* value, langbind::TypedFilterBase::Element& element)
{return printValue_long( (const signed long*)value, element);}
template <> bool printValue_<unsigned long>( const void* value, langbind::TypedFilterBase::Element& element)
{return printValue_ulong( (const unsigned long*)value, element);}
template <> bool printValue_<signed short>( const void* value, langbind::TypedFilterBase::Element& element)
{return printValue_short( (const signed short*)value, element);}
template <> bool printValue_<unsigned short>( const void* value, langbind::TypedFilterBase::Element& element)
{return printValue_ushort( (const unsigned short*)value, element);}
template <> bool printValue_<signed char>( const void* value, langbind::TypedFilterBase::Element& element)
{return printValue_char( (const signed char*)value, element);}
template <> bool printValue_<unsigned char>( const void* value, langbind::TypedFilterBase::Element& element)
{return printValue_uchar( (const unsigned char*)value, element);}
template <> bool printValue_<float>( const void* value, langbind::TypedFilterBase::Element& element)
{return printValue_float( (const float*)value, element);}
template <> bool printValue_<double>( const void* value, langbind::TypedFilterBase::Element& element)
{return printValue_double( (const double*)value, element);}
template <> bool printValue_<std::string>( const void* value, langbind::TypedFilterBase::Element& element)
{return printValue_string( (const std::string*)value, element);}

template <typename TYPE>
static bool fetchObject_( const traits::struct_&, Context& ctx, FiltermapSerializeStateStack& stk)
{
	static const StructDescriptionBase* descr = TYPE::getStructDescription();
	return fetchObjectStruct( descr, ctx, stk);
}

template <typename TYPE>
static bool fetchObject_( const traits::atomic_&, Context& ctx, FiltermapSerializeStateStack& stk)
{
	return fetchObjectAtomic( printValue_<TYPE>, ctx, stk);
}

template <typename TYPE>
static bool fetchObject_( const traits::vector_&, Context& ctx, FiltermapSerializeStateStack& stk)
{
	const std::vector<typename TYPE::value_type>* obj = (const TYPE*)stk.back().value();
	std::size_t idx = stk.back().state();
	if (idx >= obj->size())
	{
		stk.pop_back();
		return false;
	}
	const void* ve = &(*obj)[ idx];
	return fetchObjectVectorElement( &FiltermapIntrusiveSerializer<typename TYPE::value_type>::fetch, ve, ctx, stk);
}
}//anonymous namespace

template <typename TYPE>
bool FiltermapIntrusiveSerializer<TYPE>::fetch( Context& ctx, FiltermapSerializeStateStack& stk)
{
	static TYPE* t = 0;
	return fetchObject_<TYPE>( traits::getFiltermapCategory(*t), ctx, stk);
}

}}//namespace
#endif
