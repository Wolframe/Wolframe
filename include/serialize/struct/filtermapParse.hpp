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
#include "protocol/inputfilter.hpp"
#include "serialize/struct/filtermapTraits.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include "logger-v1.hpp"
#include <stdexcept>
#include <string>
#include <cstddef>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/any.hpp>
#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>

namespace _Wolframe {
namespace serialize {

template <typename T>
static void parseObject( const char* tag, T& obj, protocol::InputFilter& inp, bool);

bool isAtomic_( const filtertraits::struct_&)
{
	return false;
}
bool isAtomic_( const filtertraits::vector_&)
{
	return false;
}
bool isAtomic_( const filtertraits::arithmetic_&)
{
	return true;
}
bool isAtomic_( const filtertraits::bool_&)
{
	return true;
}

template <typename T>
bool parseObject_( const char* tag, void* obj, const filtertraits::struct_&, protocol::InputFilter& inp, Context& ctx, bool isinit)
{
	protocol::InputFilter::ElementType typ;
	const char* element;
	std::size_t elementsize;
	static const FiltermapDescriptionBase* descr = T::getFiltermapDescription();
	unsigned int depth = 0;
	std::vector<bool> isinitar( descr->end() - descr->begin(), false);

	if (isinit)
	{
		ctx.setError( tag, "duplicate structure definition");
		return false;
	}
	if (tag)
	{
		ctx.setError( tag, "atomic value expected for an attribute value");
		return false;
	}
	while (inp.getNext( typ, (const void*&) element, elementsize))
	{
		switch (typ)
		{
			case protocol::InputFilter::OpenTag:
			{
				++depth;
				FiltermapDescriptionBase::Map::const_iterator itr = descr->find( element);
				if (itr == descr->end())
				{
					ctx.setError( element, "unknown element");
					ctx.setError( tag);
					return false;
				}
				std::size_t idx = itr - descr->begin();
				if (!itr->second.parse()( 0, (char*)obj+itr->second.ofs(), inp, ctx, isinitar[ idx])) return false;
				isinitar[ idx] = true;
				if (ctx.endTagConsumed())
				{
					--depth;
					ctx.endTagConsumed(false);
					return true;
				}
				break;
			}

			case protocol::InputFilter::Attribute:
			{
				FiltermapDescriptionBase::Map::const_iterator itr = descr->find( element);
				if (itr == descr->end())
				{
					ctx.setError( element, "unknown element");
					ctx.setError( tag);
					return false;
				}
				if ((std::size_t)(itr-descr->begin()) >= descr->nof_attributes())
				{
					ctx.setError( element, "content element expected");
				}
				std::size_t idx = itr - descr->begin();
				if (!itr->second.parse()( element, (char*)obj+itr->second.ofs(), inp, ctx, isinitar[ idx])) return false;
				isinitar[ idx] = true;

				ctx.endTagConsumed(false);
				break;
			}
			case protocol::InputFilter::Value:
			{
				std::size_t ii;
				for (ii=0; ii<elementsize; ii++) if (((const unsigned char*)element)[ii]>32) break;
				if (ii==elementsize) break;
				ctx.endTagConsumed(false);
				ctx.setError( tag, "structure expected");
				return false;
			}
			case protocol::InputFilter::CloseTag:
				if (depth == 0)
				{
					ctx.endTagConsumed(true);
					return true;
				}
				--depth;
				ctx.endTagConsumed(false);
		}
	}
	return true;
}

template <typename T>
bool parseObject_( const char* tag, void* obj, const filtertraits::bool_&, protocol::InputFilter& inp, Context& ctx, bool isinit)
{
	protocol::InputFilter::ElementType typ;
	const char* element;
	std::size_t elementsize;

	if (isinit)
	{
		ctx.setError( tag, "duplicate boolean definition");
		return false;
	}
	if (inp.getNext( typ, (const void*&)element, elementsize))
	{
		switch (typ)
		{
			case protocol::InputFilter::OpenTag:
			case protocol::InputFilter::Attribute:
				ctx.setError( tag, "atomic value expected instead of structure");
				return false;

			case protocol::InputFilter::Value:
			{
				if (boost::algorithm::iequals( element, "yes")
				||  boost::algorithm::iequals( element, "y")
				||  boost::algorithm::iequals( element, "t")
				||  boost::algorithm::iequals( element, "true"))
				{
					*((T*)obj) = true;
					break;
				}
				if (boost::algorithm::iequals( element, "no")
				||  boost::algorithm::iequals( element, "n")
				||  boost::algorithm::iequals( element, "f")
				||  boost::algorithm::iequals( element, "false"))
				{
					*((T*)obj) = false;
					break;
				}
				ctx.setError( tag, "boolean value expected");
				return false;
			}
			case protocol::InputFilter::CloseTag:
				ctx.setError( tag, "boolean value expected");
				return false;
		}
		return true;
	}
	else
	{
		ctx.setError( tag, "unexpected end of document");
		return false;
	}
}

template <typename T>
bool parseObject_( const char* tag, void* obj, const filtertraits::vector_&, protocol::InputFilter& inp, Context& ctx, bool)
{
	typename T::value_type val;
	if (!parseObject( tag, val, inp, ctx, false))
	{
		ctx.setError( tag);
		return false;
	}
	((T*)obj)->push_back( val);
	ctx.endTagConsumed(false);
	return true;
}

template <typename T>
static bool parseObject_( const char* tag, void* obj, const filtertraits::arithmetic_&, protocol::InputFilter& inp, Context& ctx, bool isinit)
{
	protocol::InputFilter::ElementType typ;
	const char* element;
	std::size_t elementsize;

	if (isinit)
	{
		ctx.setError( tag, "duplicate value definition");
		return false;
	}
	if (inp.getNext( typ, (const void*&)element, elementsize))
	{
		try
		{
			switch (typ)
			{
				case protocol::InputFilter::OpenTag:
				case protocol::InputFilter::Attribute:
					ctx.setError( tag, "arithmetic value or string expected");
					return false;

				case protocol::InputFilter::Value:
					*((T*)obj) = boost::lexical_cast<T>( element);
					break;

				case protocol::InputFilter::CloseTag:
					*((T*)obj) = boost::lexical_cast<T>( "");
					ctx.endTagConsumed(true);
					break;
			}
		}
		catch (boost::bad_lexical_cast& e)
		{
			ctx.setError( tag, e.what());
			return false;
		}
	}
	else
	{
		ctx.setError( tag, "unexpected end of document");
		return false;
	}
	return true;
}

template <typename T>
static bool parseObject( const char* tag, T& obj, protocol::InputFilter& inp, Context& ctx, bool isinit)
{
	if (!_Wolframe::serialize::parseObject_<T>( tag, (void*)&obj, filtertraits::getCategory(obj), inp, ctx, isinit))
	{
		ctx.setError( tag);
		return false;
	}
	return true;
}

template <typename T>
struct FiltermapIntrusiveParser
{
	static bool isAtomic()
	{
		T* obj = 0;
		return isAtomic_( filtertraits::getCategory(*obj));
	}

	static bool parse( const char* tag, void* obj, protocol::InputFilter& inp, Context& ctx, bool isinit)
	{
		return parseObject_<T>( tag, obj, filtertraits::getCategory(*(T*)obj), inp, ctx, isinit);
	}
};

}}//namespace
#endif

