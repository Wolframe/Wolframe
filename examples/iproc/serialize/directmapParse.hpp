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
///\file serialize/directmapParse.hpp
///\brief Defines the intrusive implementation of the parsing part of serialization for the direct map
#ifndef _Wolframe_DIRECTMAP_PARSE_HPP_INCLUDED
#define _Wolframe_DIRECTMAP_PARSE_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "serialize/directmapTraits.hpp"
#include "serialize/directmapBase.hpp"
#include "logger.hpp"
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

struct ParseError
{
	std::string m_location;
	std::string m_message;

	ParseError( const char* location, const std::string& message, const char* name=0)
		:m_location(location?location:""),m_message(message)
	{
		if (name)
		{
			m_message.append( " '");
			m_message.append( name);
			m_message.append( "'");
		}
	}

	ParseError( const char* location, const ParseError& prev)
		:m_message(prev.m_message)
	{
		if (location)
		{
			m_location.append( location);
			m_location.append( "/");
		}
		m_location.append( prev.m_location);
	}
};

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
void parseObject_( const char* tag, void* obj, const struct_&, protocol::InputFilter& inp, ProcessingContext& ctx)
{
	protocol::InputFilter::ElementType typ;
	enum {bufsize=4096};
	char buf[ bufsize];
	std::size_t bufpos=0;
	static const DescriptionBase* descr = T::getDescription();
	unsigned int m_depth = 0;

	if (tag) throw ParseError( tag, "atomic value expected for an attribute value");

	try
	{
		while (inp.getNext( &typ, buf, bufsize-1, &bufpos))
		{
			buf[ bufpos] = 0;
			switch (typ)
			{
				case protocol::InputFilter::OpenTag:
				{
					++m_depth;
					DescriptionBase::Map::const_iterator itr = descr->find( buf);
					if (itr == descr->m_elem.end())
					{
						throw ParseError( tag, "unknown element ", buf);
					}
					itr->second.m_parse( 0, (char*)obj+itr->second.m_ofs, inp, ctx);
					if (ctx.endTagConsumed())
					{
						--m_depth;
						ctx.endTagConsumed(false);
					}
					break;
				}

				case protocol::InputFilter::Attribute:
				{
					DescriptionBase::Map::const_iterator itr = descr->find( buf);
					if (itr == descr->m_elem.end())
					{
						throw ParseError( tag, "unknown element ", buf);
					}
					itr->second.m_parse( buf, (char*)obj+itr->second.m_ofs, inp, ctx);
					ctx.endTagConsumed(false);
					break;
				}
				case protocol::InputFilter::Value:
					ctx.endTagConsumed(false);
					throw ParseError( tag, "structure expected");

				case protocol::InputFilter::CloseTag:
					if (m_depth == 0)
					{
						ctx.endTagConsumed(true);
						return;
					}
					--m_depth;
					ctx.endTagConsumed(false);
			}
			bufpos = 0;
		}
	}
	catch (const ParseError& e)
	{
		throw ParseError( tag, e);
	}
	catch (boost::bad_lexical_cast& e)
	{
		throw ParseError( tag, "value not in domain", e.what());
	}
	catch (std::bad_alloc& e)
	{
		throw ParseError( tag, "out of memory");
	}
}

template <typename T>
void parseObject_( const char* tag, void* obj, const bool_&, protocol::InputFilter& inp, ProcessingContext&)
{
	protocol::InputFilter::ElementType typ;
	enum {bufsize=4096};
	char buf[ bufsize];
	std::size_t bufpos=0;

	if (inp.getNext( &typ, buf, bufsize-1, &bufpos))
	{
		buf[ bufpos] = 0;
		switch (typ)
		{
			case protocol::InputFilter::OpenTag:
			case protocol::InputFilter::Attribute:
				throw ParseError( tag, "atomic value expected instead of structure");

			case protocol::InputFilter::Value:
			{
				if (boost::algorithm::iequals( buf, "yes")
				||  boost::algorithm::iequals( buf, "y")
				||  boost::algorithm::iequals( buf, "t")
				||  boost::algorithm::iequals( buf, "true"))
				{
					*((T*)obj) = true;
					break;
				}
				if (boost::algorithm::iequals( buf, "no")
				||  boost::algorithm::iequals( buf, "n")
				||  boost::algorithm::iequals( buf, "f")
				||  boost::algorithm::iequals( buf, "false"))
				{
					*((T*)obj) = false;
					break;
				}
				throw ParseError( tag, "boolean value expected");
			}
			case protocol::InputFilter::CloseTag:
				throw ParseError( tag, "boolean value expected");
		}
	}
	else
	{
		throw ParseError( tag, "unexpected end of document");
	}
}

template <typename T>
void parseObject_( const char* tag, void* obj, const vector_&, protocol::InputFilter& inp, ProcessingContext& ctx)
{
	typename T::value_type val;
	parseObject( tag, val, inp, ctx);
	((T*)obj)->push_back( val);
}

template <typename T>
static void parseObject_( const char* tag, void* obj, const arithmetic_&, protocol::InputFilter& inp, ProcessingContext& ctx)
{
	protocol::InputFilter::ElementType typ;
	enum {bufsize=4096};
	char buf[ bufsize];
	std::size_t bufpos=0;

	if (inp.getNext( &typ, buf, bufsize-1, &bufpos))
	{
		buf[ bufpos] = 0;
		switch (typ)
		{
			case protocol::InputFilter::OpenTag:
			case protocol::InputFilter::Attribute:
				throw ParseError( tag, "arithmetic value or string expected");

			case protocol::InputFilter::Value:
			{
				*((T*)obj) = boost::lexical_cast<T>( buf);
				break;
			}
			case protocol::InputFilter::CloseTag:
				*((T*)obj) = boost::lexical_cast<T>( "");
				ctx.endTagConsumed(true);
				break;
		}
	}
	else
	{
		throw ParseError( tag, "unexpected end of document");
	}
}

template <typename T>
static void parseObject( const char* tag, T& obj, protocol::InputFilter& inp, ProcessingContext& ctx)
{
	_Wolframe::serialize::parseObject_<T>( tag, (void*)&obj, getCategory(obj), inp, ctx);
}

template <typename T>
struct IntrusiveParser
{
	static bool isAtomic()
	{
		T* obj = 0;
		return isAtomic_( getCategory(*obj));
	}

	static void parse( const char* tag, void* obj, protocol::InputFilter& inp, ProcessingContext& ctx)
	{
		try
		{
			parseObject_<T>( tag, obj, getCategory(*(T*)obj), inp, ctx);
		}
		catch (ParseError& e)
		{
			std::string msg( "/");
			msg.append( e.m_location);
			msg.append( ": ");
			msg.append( e.m_message);
			throw std::runtime_error( msg.c_str());
		}
	}
};

}}//namespace
#endif

