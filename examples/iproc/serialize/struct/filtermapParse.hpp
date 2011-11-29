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

///the intrusive part of the definitions is put into an anonymous namespace:
namespace _Wolframe {
namespace serialize {

template <typename T>
static void parseObject( const char* tag, T& obj, protocol::InputFilter& inp);

bool isAtomic_( const struct_&)
{
	return false;
}
bool isAtomic_( const vector_&)
{
	return false;
}
bool isAtomic_( const arithmetic_&)
{
	return true;
}
bool isAtomic_( const bool_&)
{
	return true;
}

template <typename T>
bool parseObject_( const char* tag, void* obj, const struct_&, protocol::InputFilter& inp, Context& ctx)
{
	protocol::InputFilter::ElementType typ;
	static const DescriptionBase* descr = T::getDescription();
	unsigned int depth = 0;
	std::size_t bufpos = 0;

	if (tag)
	{
		ctx.setError( tag, "atomic value expected for an attribute value");
		return false;
	}
	while (inp.getNext( &typ, ctx.buf(), Context::bufsize-1, &bufpos))
	{
		ctx.buf()[ bufpos] = 0;
		switch (typ)
		{
			case protocol::InputFilter::OpenTag:
			{
				++depth;
				DescriptionBase::Map::const_iterator itr = descr->find( ctx.buf());
				if (itr == descr->end())
				{
					ctx.setError( ctx.buf(), "unknown element");
					ctx.setError( tag);
					return false;
				}
				if (!itr->second.parse()( 0, (char*)obj+itr->second.ofs(), inp, ctx)) return false;
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
				DescriptionBase::Map::const_iterator itr = descr->find( ctx.buf());
				if (itr == descr->end())
				{
					ctx.setError( ctx.buf(), "unknown element");
					ctx.setError( tag);
					return false;
				}
				if (!itr->second.parse()( ctx.buf(), (char*)obj+itr->second.ofs(), inp, ctx)) return false;
				ctx.endTagConsumed(false);
				break;
			}
			case protocol::InputFilter::Value:
			{
				std::size_t ii;
				for (ii=0; ii<bufpos; ii++) if (ctx.buf()[ii]>32) break;
				if (ii==bufpos) break;
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
		bufpos = 0;
	}
	return true;
}

template <typename T>
bool parseObject_( const char* tag, void* obj, const bool_&, protocol::InputFilter& inp, Context& ctx)
{
	protocol::InputFilter::ElementType typ;
	std::size_t bufpos=0;

	if (inp.getNext( &typ, ctx.buf(), Context::bufsize-1, &bufpos))
	{
		ctx.buf()[ bufpos] = 0;
		switch (typ)
		{
			case protocol::InputFilter::OpenTag:
			case protocol::InputFilter::Attribute:
				ctx.setError( tag, "atomic value expected instead of structure");
				return false;

			case protocol::InputFilter::Value:
			{
				if (boost::algorithm::iequals( ctx.buf(), "yes")
				||  boost::algorithm::iequals( ctx.buf(), "y")
				||  boost::algorithm::iequals( ctx.buf(), "t")
				||  boost::algorithm::iequals( ctx.buf(), "true"))
				{
					*((T*)obj) = true;
					break;
				}
				if (boost::algorithm::iequals( ctx.buf(), "no")
				||  boost::algorithm::iequals( ctx.buf(), "n")
				||  boost::algorithm::iequals( ctx.buf(), "f")
				||  boost::algorithm::iequals( ctx.buf(), "false"))
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
bool parseObject_( const char* tag, void* obj, const vector_&, protocol::InputFilter& inp, Context& ctx)
{
	typename T::value_type val;
	if (!parseObject( tag, val, inp, ctx))
	{
		ctx.setError( tag);
		return false;
	}
	((T*)obj)->push_back( val);
	ctx.endTagConsumed(false);
	return true;
}

template <typename T>
static bool parseObject_( const char* tag, void* obj, const arithmetic_&, protocol::InputFilter& inp, Context& ctx)
{
	protocol::InputFilter::ElementType typ;
	std::size_t bufpos=0;

	if (inp.getNext( &typ, ctx.buf(), Context::bufsize-1, &bufpos))
	{
		ctx.buf()[ bufpos] = 0;
		try
		{
			switch (typ)
			{
				case protocol::InputFilter::OpenTag:
				case protocol::InputFilter::Attribute:
					ctx.setError( tag, "arithmetic value or string expected");
					return false;

				case protocol::InputFilter::Value:
					*((T*)obj) = boost::lexical_cast<T>( ctx.buf());
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
static bool parseObject( const char* tag, T& obj, protocol::InputFilter& inp, Context& ctx)
{
	if (!_Wolframe::serialize::parseObject_<T>( tag, (void*)&obj, getCategory(obj), inp, ctx))
	{
		ctx.setError( tag);
		return false;
	}
	return true;
}

template <typename T>
struct IntrusiveParser
{
	static bool isAtomic()
	{
		T* obj = 0;
		return isAtomic_( getCategory(*obj));
	}

	static bool parse( const char* tag, void* obj, protocol::InputFilter& inp, Context& ctx)
	{
		return parseObject_<T>( tag, obj, getCategory(*(T*)obj), inp, ctx);
	}
};

}}//namespace
#endif

