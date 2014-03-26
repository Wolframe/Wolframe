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
///\file serialize/struct/intrusiveSerializer.hpp
///\brief Defines the intrusive implementation of the serialization of structures
#ifndef _Wolframe_SERLIALIZE_STRUCT_INTRUSIVE_SERIALIZER_HPP_INCLUDED
#define _Wolframe_SERLIALIZE_STRUCT_INTRUSIVE_SERIALIZER_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "types/bignumber.hpp"
#include "types/datetime.hpp"
#include "serialize/struct/traits_getCategory.hpp"
#include "serialize/struct/structDescriptionBase.hpp"
#include "serialize/struct/serializeStack.hpp"
#include <string>
#include <vector>
#include <map>
#include <boost/cstdint.hpp>

namespace _Wolframe {
namespace serialize {

// forward declaration
template <typename TYPE>
struct IntrusiveSerializer
{
	static bool fetch( Context& ctx, SerializeStateStack& stk);
};

bool fetchCloseTag( Context& ctx, SerializeStateStack& stk);

bool fetchOpenTag( Context& ctx, SerializeStateStack& stk);

bool fetchObjectStruct( const StructDescriptionBase* descr, Context& ctx, SerializeStateStack& stk);

typedef bool (*PrintValue)( const void* ptr, types::VariantConst& value);

bool fetchObjectAtomic( PrintValue prnt, Context& ctx, SerializeStateStack& stk);

typedef bool (*PrintValue)( const void* value, types::VariantConst& element);

bool printValue_int64( const boost::int64_t*, types::VariantConst& element);
bool printValue_uint64( const boost::uint64_t*, types::VariantConst& element);
bool printValue_int32( const boost::int32_t*, types::VariantConst& element);
bool printValue_uint32( const boost::uint32_t*, types::VariantConst& element);
bool printValue_short( const signed short*, types::VariantConst& element);
bool printValue_ushort( const unsigned short*, types::VariantConst& element);
bool printValue_char( const signed char*, types::VariantConst& element);
bool printValue_uchar( const unsigned char*, types::VariantConst& element);
bool printValue_bool( const bool*, types::VariantConst& element);
bool printValue_float( const float*, types::VariantConst& element);
bool printValue_double( const double*, types::VariantConst& element);
bool printValue_string( const std::string*, types::VariantConst& element);
bool printValue_datetime( const types::DateTime*, types::VariantConst& element);
bool printValue_bignumber( const types::BigNumber*, types::VariantConst& element);

typedef bool (*FetchElement)( Context& ctx, SerializeStateStack& stk);

bool fetchObjectVectorElement( FetchElement fetchElement, const void* ve, Context& ctx, SerializeStateStack& stk);

namespace {
template <typename TYPE> struct Printer {};

template <> struct Printer<boost::int64_t>
{static bool printValue( const void* value, types::VariantConst& element) {return printValue_int64( (const boost::int64_t*)value, element);}};
template <> struct Printer<boost::uint64_t>
{static bool printValue( const void* value, types::VariantConst& element) {return printValue_uint64( (const boost::uint64_t*)value, element);}};

template <> struct Printer<boost::int32_t>
{static bool printValue( const void* value, types::VariantConst& element) {return printValue_int32( (const boost::int32_t*)value, element);}};
template <> struct Printer<boost::uint32_t>
{static bool printValue( const void* value, types::VariantConst& element) {return printValue_uint32( (const boost::uint32_t*)value, element);}};

template <> struct Printer<boost::int16_t>
{static bool printValue( const void* value, types::VariantConst& element) {return printValue_short( (const boost::int16_t*)value, element);}};
template <> struct Printer<boost::uint16_t>
{static bool printValue( const void* value, types::VariantConst& element) {return printValue_ushort( (const boost::uint16_t*)value, element);}};

template <> struct Printer<signed char>
{static bool printValue( const void* value, types::VariantConst& element) {return printValue_char( (const signed char*)value, element);}};
template <> struct Printer<unsigned char>
{static bool printValue( const void* value, types::VariantConst& element) {return printValue_uchar( (const unsigned char*)value, element);}};

template <> struct Printer<bool>
{static bool printValue( const void* value, types::VariantConst& element) {return printValue_bool( (const bool*)value, element);}};

template <> struct Printer<float>
{static bool printValue( const void* value, types::VariantConst& element) {return printValue_float( (const float*)value, element);}};

template <> struct Printer<double>
{static bool printValue( const void* value, types::VariantConst& element) {return printValue_double( (const double*)value, element);}};

template <> struct Printer<std::string>
{static bool printValue( const void* value, types::VariantConst& element) {return printValue_string( (const std::string*)value, element);}};

template <> struct Printer<types::DateTime>
{static bool printValue( const void* value, types::VariantConst& element) {return printValue_datetime( (const types::DateTime*)value, element);}};

template <> struct Printer<types::BigNumber>
{static bool printValue( const void* value, types::VariantConst& element) {return printValue_bignumber( (const types::BigNumber*)value, element);}};


template <typename TYPE>
static bool fetchObject_( const traits::struct_&, Context& ctx, SerializeStateStack& stk)
{
	static const StructDescriptionBase* descr = TYPE::getStructDescription();
	return fetchObjectStruct( descr, ctx, stk);
}

template <typename TYPE>
static bool fetchObject_( const traits::atomic_&, Context& ctx, SerializeStateStack& stk)
{
	return fetchObjectAtomic( Printer<TYPE>::printValue, ctx, stk);
}

template <typename TYPE>
static bool fetchObject_( const traits::vector_&, Context& ctx, SerializeStateStack& stk)
{
	const std::vector<typename TYPE::value_type>* obj = (const TYPE*)stk.back().value();
	std::size_t idx = stk.back().state();
	if (idx >= obj->size())
	{
		stk.pop_back();
		return false;
	}
	const void* ve = &(*obj)[ idx];
	return fetchObjectVectorElement( &IntrusiveSerializer<typename TYPE::value_type>::fetch, ve, ctx, stk);
}
}//anonymous namespace

template <typename TYPE>
bool IntrusiveSerializer<TYPE>::fetch( Context& ctx, SerializeStateStack& stk)
{
	static TYPE* t = 0;
	return fetchObject_<TYPE>( traits::getCategory(*t), ctx, stk);
}

}}//namespace
#endif
