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
#include "filter/inputfilter.hpp"
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
static void parseObject( T& obj, langbind::InputFilter& inp, bool);

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
bool parseObject_( void* obj, const filtertraits::struct_&, langbind::InputFilter& inp, Context& ctx, bool isinit)
{
	langbind::InputFilter::ElementType typ;
	const char* element;
	std::size_t elementsize;
	static const FiltermapDescriptionBase* descr = T::getFiltermapDescription();
	std::vector<bool> isinitar( descr->end() - descr->begin(), false);

	if (isinit)
	{
		ctx.setError( "duplicate structure definition");
		return false;
	}
	while (ctx.followTagConsumed() || inp.getNext( typ, (const void*&) element, elementsize))
	{
		ctx.followTagConsumed(false);
		switch (typ)
		{
			case langbind::InputFilter::OpenTag:
			{
				FiltermapDescriptionBase::Map::const_iterator itr = descr->find( (const char*)element);
				if (itr == descr->end())
				{
					ctx.setError( "unknown element", element);
					return false;
				}
				std::size_t idx = itr - descr->begin();
				if (!itr->second.parse()( (char*)obj+itr->second.ofs(), inp, ctx, isinitar[ idx]))
				{
					ctx.setTag( element);
					return false;
				}
				isinitar[ idx] = true;
				if (!ctx.followTagConsumed())
				{
					if (!inp.getNext( typ, (const void*&) element, elementsize) || typ != langbind::InputFilter::CloseTag)
					{
						ctx.setError( "incomplete element. tag close expected");
						return false;
					}
				}
				else
				{
					ctx.followTagConsumed(false);
				}
				break;
			}

			case langbind::InputFilter::Attribute:
			{
				FiltermapDescriptionBase::Map::const_iterator itr = descr->find( (const char*)element);
				if (itr == descr->end())
				{
					ctx.setError( "unknown attribute", element);
					return false;
				}
				if ((std::size_t)(itr-descr->begin()) >= descr->nof_attributes())
				{
					ctx.setError( "content element expected", element);
					return false;
				}
				std::size_t idx = itr - descr->begin();
				if (!itr->second.parse()( (char*)obj+itr->second.ofs(), inp, ctx, isinitar[ idx]))
				{
					ctx.setTag( element);
					return false;
				}
				isinitar[ idx] = true;
				break;
			}
			case langbind::InputFilter::Value:
			{
				std::size_t ii;
				for (ii=0; ii<elementsize; ii++) if (((const unsigned char*)element)[ii]>32) break;
				if (ii==elementsize) break;
				ctx.setError( "structure expected");
				return false;
			}
			case langbind::InputFilter::CloseTag:
				ctx.followTagConsumed(true);
				return true;
		}
	}
	if (inp.state() == langbind::InputFilter::Error)
	{
		ctx.setError( inp.getError());
		return false;
	}
	return (inp.state() == langbind::InputFilter::Open);
}

template <typename T>
bool parseObject_( void* obj, const filtertraits::bool_&, langbind::InputFilter& inp, Context& ctx, bool isinit)
{
	langbind::InputFilter::ElementType typ;
	const char* element;
	std::size_t elementsize;

	if (isinit)
	{
		ctx.setError( "duplicate boolean definition");
		return false;
	}
	if (inp.getNext( typ, (const void*&)element, elementsize))
	{
		switch (typ)
		{
			case langbind::InputFilter::OpenTag:
			case langbind::InputFilter::Attribute:
				ctx.setError( "atomic value expected instead of structure");
				return false;

			case langbind::InputFilter::Value:
			{
				if (boost::algorithm::iequals( element, "yes")
				||  boost::algorithm::iequals( element, "y")
				||  boost::algorithm::iequals( element, "t")
				||  boost::algorithm::iequals( element, "true"))
				{
					ctx.followTagConsumed(false);
					*((T*)obj) = true;
					break;
				}
				if (boost::algorithm::iequals( element, "no")
				||  boost::algorithm::iequals( element, "n")
				||  boost::algorithm::iequals( element, "f")
				||  boost::algorithm::iequals( element, "false"))
				{
					ctx.followTagConsumed(false);
					*((T*)obj) = false;
					break;
				}
				ctx.setError( "boolean value expected");
				return false;
			}
			case langbind::InputFilter::CloseTag:
				ctx.followTagConsumed(true);
				ctx.setError( "boolean value expected");
				return false;
		}
		return (inp.state() == langbind::InputFilter::Open);
	}
	else if (inp.state() == langbind::InputFilter::Error)
	{
		ctx.setError( inp.getError());
		return false;
	}
	else
	{
		ctx.setError( "unexpected end of document");
		return false;
	}
}

template <typename T>
bool parseObject_( void* obj, const filtertraits::vector_&, langbind::InputFilter& inp, Context& ctx, bool)
{
	typename T::value_type val;

	if (!parseObject( val, inp, ctx, false))
	{
		return false;
	}
	((T*)obj)->push_back( val);
	return true;
}

template <typename T>
static bool parseObject_( void* obj, const filtertraits::arithmetic_&, langbind::InputFilter& inp, Context& ctx, bool isinit)
{
	langbind::InputFilter::ElementType typ;
	const char* element;
	std::size_t elementsize;

	if (isinit)
	{
		ctx.setError( "duplicate value definition");
		return false;
	}
	if (inp.getNext( typ, (const void*&)element, elementsize))
	{
		try
		{
			switch (typ)
			{
				case langbind::InputFilter::OpenTag:
				case langbind::InputFilter::Attribute:
					ctx.setError( "arithmetic value or string expected");
					return false;

				case langbind::InputFilter::Value:
					ctx.followTagConsumed(false);
					*((T*)obj) = boost::lexical_cast<T>( element);
					break;

				case langbind::InputFilter::CloseTag:
					*((T*)obj) = boost::lexical_cast<T>( "");
					ctx.followTagConsumed(true);
					break;
			}
			return true;
		}
		catch (boost::bad_lexical_cast& e)
		{
			ctx.setError( e.what());
			return false;
		}
	}
	else if (inp.state() == langbind::InputFilter::Error)
	{
		ctx.setError( inp.getError());
		return false;
	}
	else
	{
		ctx.setError( "unexpected end of document");
		return false;
	}
}

template <typename T>
static bool parseObject( T& obj, langbind::InputFilter& inp, Context& ctx, bool isinit)
{
	if (!_Wolframe::serialize::parseObject_<T>( (void*)&obj, filtertraits::getCategory(obj), inp, ctx, isinit))
	{
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

	static bool parse( void* obj, langbind::InputFilter& inp, Context& ctx, bool isinit)
	{
		return parseObject_<T>( obj, filtertraits::getCategory(*(T*)obj), inp, ctx, isinit);
	}
};

}}//namespace
#endif

