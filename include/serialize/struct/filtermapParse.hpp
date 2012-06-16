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
///\file serialize/struct/filtermapParse.hpp
///\brief Defines the intrusive implementation of the parsing part of serialization/deserialization of filters
#ifndef _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PARSE_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PARSE_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "serialize/struct/filtermapTraits.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include "serialize/struct/filtermapParseStack.hpp"
#include "serialize/struct/filtermapParseValue.hpp"
#include "serialize/struct/filtermapProperty.hpp"
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstddef>
#include <boost/lexical_cast.hpp>
#include <boost/utility/value_init.hpp>

namespace _Wolframe {
namespace serialize {

// forward declaration
template <typename T>
struct FiltermapIntrusiveParser
{
	static bool parse( langbind::TypedInputFilter& inp, Context& ctx, FiltermapParseStateStack& stk);
};

static bool parseAtomicElementEndTag( langbind::TypedInputFilter& inp, Context& ctx, FiltermapParseStateStack& stk)
{
	langbind::InputFilter::ElementType typ;
	langbind::TypedFilterBase::Element element;

	if (!inp.getNext( typ, element))
	{
		const char* err = inp.getError();
		if (err) ctx.setError( err);
		return false;
	}
	if (typ == langbind::InputFilter::Value)
	{
		ctx.setError( "subsequent atomic values not allowed in filter serialization");
		return false;
	}
	if (typ != langbind::InputFilter::CloseTag)
	{
		ctx.setError( "close tag expected after atomic element");
		return false;
	}
	stk.pop_back();
	return true;
}

template <typename T>
bool parseObject_( const traits::struct_&, langbind::TypedInputFilter& inp, Context& ctx, FiltermapParseStateStack& stk)
{
	langbind::InputFilter::ElementType typ;
	langbind::TypedFilterBase::Element element;
	static const FiltermapDescriptionBase* descr = T::getFiltermapDescription();

	if (!inp.getNext( typ, element))
	{
		const char* err = inp.getError();
		if (err) ctx.setError( err);
		return false;
	}
	switch (typ)
	{
		case langbind::InputFilter::OpenTag:
		{
			FiltermapDescriptionBase::Map::const_iterator itr = descr->find( element.tostring());
			if (itr == descr->end())
			{
				ctx.setError( "unknown element ", element.tostring());
				return false;
			}
			std::size_t idx = itr - descr->begin();
			if (idx < descr->nof_attributes())
			{
				if (ctx.flag( Context::ValidateAttributes))
				{
					ctx.setError( "attribute element expected");
					return false;
				}
			}
			if (itr->second.type() != FiltermapDescriptionBase::Vector)
			{
				if (stk.back().selectElement( idx, descr->size()))
				{
					ctx.setError( "duplicate definition");
					return false;
				}
			}
			void* value = (char*)stk.back().value() + itr->second.ofs();
			if (itr->second.type() == FiltermapDescriptionBase::Atomic)
			{
				stk.push_back( FiltermapParseState( 0, &parseAtomicElementEndTag, value));
			}
			stk.push_back( FiltermapParseState( itr->first.c_str(), itr->second.parse(), value));
			return true;
		}

		case langbind::InputFilter::Attribute:
		{
			FiltermapDescriptionBase::Map::const_iterator itr = descr->find( element.tostring());
			if (itr == descr->end())
			{
				ctx.setError( "unknown attribute");
				return false;
			}
			std::size_t idx = itr - descr->begin();
			if (idx >= descr->nof_attributes())
			{
				if (ctx.flag( Context::ValidateAttributes))
				{
					ctx.setError( "content element expected");
					return false;
				}
			}
			if (itr->second.type() != FiltermapDescriptionBase::Atomic)
			{
				ctx.setError( "atomic value expected for attribute");
				return false;
			}
			if (stk.back().selectElement( idx, descr->size()))
			{
				ctx.setError( "duplicate definition");
				return false;
			}
			stk.push_back( FiltermapParseState( itr->first.c_str(), itr->second.parse(), (char*)stk.back().value() + itr->second.ofs()));
			return true;
		}

		case langbind::InputFilter::Value:
		{
			ctx.setError( "structure instead of value expected");
			return false;
		}

		case langbind::InputFilter::CloseTag:
		{
			FiltermapDescriptionBase::Map::const_iterator itr = descr->begin(),end=descr->end();
			for (;itr != end; ++itr)
			{
				if (itr->second.mandatory() && !stk.back().initCount( itr-descr->begin()))
				{
					ctx.setError( "undefined structure element");
					return false;
				}
			}
			stk.pop_back();
			return true;
		}
	}
	ctx.setError( "illegal state in parse structure");
	return false;
}

template <typename T>
bool parseObject_( const traits::vector_&, langbind::TypedInputFilter&, Context&, FiltermapParseStateStack& stk)
{
	if (stk.back().state())
	{
		stk.pop_back();
		return true;
	}
	typedef typename T::value_type Element;
	boost::value_initialized<Element> val;
	((T*)stk.back().value())->push_back( val);
	stk.back().state( 1);
	Element* ee = &((T*)(char*)stk.back().value())->back();
	if (FiltermapIntrusiveProperty<Element>::type() == FiltermapDescriptionBase::Atomic)
	{
		stk.push_back( FiltermapParseState( 0, &parseAtomicElementEndTag, 0));
	}
	stk.push_back( FiltermapParseState( 0, &FiltermapIntrusiveParser<Element>::parse, ee));
	return true;
}


template <typename T>
static bool parseObject_( const traits::atomic_&, langbind::TypedInputFilter& inp, Context& ctx, FiltermapParseStateStack& stk)
{
	langbind::InputFilter::ElementType typ;
	langbind::TypedFilterBase::Element element;
	T* obj = (T*)(stk.back().value());

	if (!inp.getNext( typ, element))
	{
		const char* err = inp.getError();
		if (err) ctx.setError( err);
		return false;
	}
	switch (typ)
	{
		case langbind::InputFilter::OpenTag:
		case langbind::InputFilter::Attribute:
			ctx.setError( "atomic value expected");
			return false;

		case langbind::InputFilter::Value:
			if (!parseValue( *obj, element))
			{
				ctx.setError( "illegal type of atomic value");
				return false;
			}
			stk.pop_back();
			return true;

		case langbind::InputFilter::CloseTag:
			element = langbind::TypedFilterBase::Element();
			if (!parseValue( *obj, element))
			{
				ctx.setError( "cannot convert empty value to expected atomic type");
				return false;
			}
			stk.pop_back();
			if (stk.back().parse() == &parseAtomicElementEndTag)
			{
				stk.pop_back();
			}
			else
			{
				ctx.setError( "value expected after attribute");
			}
			return true;
	}
	ctx.setError( "illegal state in parse atomic value");
	return false;
}

template <typename T>
bool FiltermapIntrusiveParser<T>::parse( langbind::TypedInputFilter& inp, Context& ctx, FiltermapParseStateStack& stk)
{
	static T* t = 0;
	return parseObject_<T>( traits::getFiltermapCategory(*t), inp, ctx, stk);
}

}}//namespace
#endif

