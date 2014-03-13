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
///\file serialize/struct/intrusiveParser.hpp
///\brief Defines the intrusive implementation of the deserialization of objects interfaced as TypedInputFilter
#ifndef _Wolframe_SERIALIZE_STRUCT_INTRUSIVE_PARSER_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_INTRUSIVE_PARSER_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "types/variant.hpp"
#include "types/datetime.hpp"
#include "types/bignumber.hpp"
#include "types/customDataType.hpp"
#include "serialize/struct/traits_getCategory.hpp"
#include "serialize/struct/structDescriptionBase.hpp"
#include "serialize/struct/parseStack.hpp"
#include "serialize/struct/intrusiveProperty.hpp"
#include <string>
#include <cstddef>
#include <boost/utility/value_init.hpp>
#include <boost/cstdint.hpp>

namespace _Wolframe {
namespace serialize {

// forward declaration
template <typename TYPE>
struct IntrusiveParser
{
	static bool parse( langbind::TypedInputFilter& inp, Context& ctx, ParseStateStack& stk);
};

bool parseAtomicElementEndTag( langbind::TypedInputFilter& inp, Context&, ParseStateStack& stk);

bool parseObjectStruct( const StructDescriptionBase* descr, langbind::TypedInputFilter& inp, Context& ctx, ParseStateStack& stk);

typedef bool (*ParseValue)( void* value, const types::VariantConst& element);

bool parseValue_int64( boost::int64_t* value, const types::VariantConst& element);
bool parseValue_uint64( boost::uint64_t* value, const types::VariantConst& element);
bool parseValue_int32( boost::int32_t* value, const types::VariantConst& element);
bool parseValue_uint32( boost::uint32_t* value, const types::VariantConst& element);
bool parseValue_short( signed short*, const types::VariantConst& element);
bool parseValue_ushort( unsigned short*, const types::VariantConst& element);
bool parseValue_char( signed char*, const types::VariantConst& element);
bool parseValue_uchar( unsigned char*, const types::VariantConst& element);
bool parseValue_float( float*, const types::VariantConst& element);
bool parseValue_double( double*, const types::VariantConst& element);
bool parseValue_string( std::string*, const types::VariantConst& element);
bool parseValue_datetime( types::DateTime*, const types::VariantConst& element);
bool parseValue_bignumber( types::BigNumber*, const types::VariantConst& element);

bool parseObjectAtomic( ParseValue parseVal, langbind::TypedInputFilter& inp, Context&, ParseStateStack& stk);


namespace {
template <typename TYPE>
static bool parseValue_( void*, const types::VariantConst&)
{throw std::runtime_error( "unable to parse atomic value of this type");}

template <> bool parseValue_<boost::int64_t>( void* value, const types::VariantConst& element)
{return parseValue_int64( (boost::int64_t*)value, element);}
template <> bool parseValue_<boost::uint64_t>( void* value, const types::VariantConst& element)
{return parseValue_uint64( (boost::uint64_t*)value, element);}

template <> bool parseValue_<boost::int32_t>( void* value, const types::VariantConst& element)
{return parseValue_int32( (boost::int32_t*)value, element);}
template <> bool parseValue_<boost::uint32_t>( void* value, const types::VariantConst& element)
{return parseValue_uint32( (boost::uint32_t*)value, element);}

template <> bool parseValue_<signed short>( void* value, const types::VariantConst& element)
{return parseValue_short( (signed short*)value, element);}
template <> bool parseValue_<unsigned short>( void* value, const types::VariantConst& element)
{return parseValue_ushort( (unsigned short*)value, element);}
template <> bool parseValue_<signed char>( void* value, const types::VariantConst& element)
{return parseValue_char( (signed char*)value, element);}
template <> bool parseValue_<unsigned char>( void* value, const types::VariantConst& element)
{return parseValue_uchar( (unsigned char*)value, element);}
template <> bool parseValue_<float>( void* value, const types::VariantConst& element)
{return parseValue_float( (float*)value, element);}
template <> bool parseValue_<double>( void* value, const types::VariantConst& element)
{return parseValue_double( (double*)value, element);}
template <> bool parseValue_<std::string>( void* value, const types::VariantConst& element)
{return parseValue_string( (std::string*)value, element);}
template <> bool parseValue_<types::DateTime>( void* value, const types::VariantConst& element)
{return parseValue_datetime( (types::DateTime*)value, element);}
template <> bool parseValue_<types::BigNumber>( void* value, const types::VariantConst& element)
{return parseValue_bignumber( (types::BigNumber*)value, element);}

template <typename TYPE>
bool parseObject_( const traits::struct_&, langbind::TypedInputFilter& inp, Context& ctx, ParseStateStack& stk)
{
	return parseObjectStruct( TYPE::getStructDescription(), inp, ctx, stk);
}

template <typename TYPE>
bool parseObject_( const traits::vector_&, langbind::TypedInputFilter&, Context&, ParseStateStack& stk)
{
	if (stk.back().state())
	{
		stk.pop_back();
		return true;
	}
	typedef typename TYPE::value_type Element;
	boost::value_initialized<Element> val;
	((TYPE*)stk.back().value())->push_back( val);
	stk.back().state( 1);
	Element* ee = &((TYPE*)stk.back().value())->back();
	if (IntrusiveProperty<Element>::type() == StructDescriptionBase::Atomic)
	{
		stk.push_back( ParseState( 0, &parseAtomicElementEndTag, 0));
	}
	stk.push_back( ParseState( 0, &IntrusiveParser<Element>::parse, ee));
	return true;
}

template <typename TYPE>
static bool parseObject_( const traits::atomic_&, langbind::TypedInputFilter& inp, Context& ctx, ParseStateStack& stk)
{
	return parseObjectAtomic( parseValue_<TYPE>, inp, ctx, stk);
}

}//anonymous namespace


template <typename TYPE>
bool IntrusiveParser<TYPE>::parse( langbind::TypedInputFilter& inp, Context& ctx, ParseStateStack& stk)
{
	static TYPE* t = 0;
	return parseObject_<TYPE>( traits::getCategory(*t), inp, ctx, stk);
}

}}//namespace
#endif

