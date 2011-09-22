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
///\file parse/directmapParse.hpp
///\brief Defines the intrusive implementation of the parsing part of serialization for the direct map
#ifndef _Wolframe_DIRECTMAP_PARSE_HPP_INCLUDED
#define _Wolframe_DIRECTMAP_PARSE_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include <stdexcept>
#include <string>
#include <cstdef>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>

///the intrusive part of the definitions is put into an anonymous namespace:
namespace {

struct ParseError
{
	std::string m_location;
	std::string m_message;

	ParseError( const char* location, const std::string& message)
		:m_location(location?location:""),m_message(message){}

	ParseError( const char* location, const ParseError& prev)
		:m_message(prev.m_message)
	{
		if (location)
		{
			m_location.append( location);
			if (!prev.m_location.empty())
			{
				m_location.append( ".");
			}
		}
		m_location.append( prev.m_location);
	}
};

struct EndTag {};

template <typename T>
void parse_( const char* tag, void* obj, struct_&, protocol::InputFilter& flt)
{
	ElementType typ;
	enum {bufsize=4096};
	char buf[ bufsize];
	std::size_t bufpos;
	static const DescriptionBase* descr = T::description();

	if (tag) throw ParseError( tag, "atomic value expected for an attribute value");

	try
	{
		while (inp->getNext( &typ, buf, bufsize-1, &bufpos))
		{
			switch (typ)
			{
				case OpenTag:
				{
					const std::string tagnam( buf, bufpos);
					std::map<std::string,DescriptionBase>::const_iterator itr = descr->m_elem->find( tagnam);
					if (itr != descr->m_elem->end()
					{
						itr->m_parse( 0, (char*)obj+itr->m_ofs, flt);
					}
					break;
				}

				case Attribute:
				{
					const std::string tagnam( buf, bufpos);
					std::map<std::string,DescriptionBase>::const_iterator itr = descr->m_elem->find( name);
					if (itr != descr->m_elem->end()
					{
						itr->m_parse( name, (char*)obj+itr->m_ofs, flt);
					}
					break;
				}
				case Value:
					throw ParseError( tag, "structure expected");

				case CloseTag:
					return;
			}
		}
	}
	catch (const ParseError& e)
	{
		throw ParseError( tag, e);
	}
	catch (boost::bad_cast& e)
	{
		throw ParseError( tag, "value not in domain");
	}
	catch (std::bad_alloc& e)
	{
		throw ParseError( tag, "out of memory");
	}
	catch (const EndTag& e)
	{}
}

template <typename T>
void parse_( const char* tag, void* obj, arithmetic_&, protocol::InputFilter& flt)
{
	ElementType typ;
	enum {bufsize=4096};
	char buf[ bufsize];
	std::size_t bufpos;

	if (inp->getNext( &typ, buf, bufsize-1, &bufpos))
	{
		switch (typ)
		{
			case OpenTag:
			case Attribute:
				throw ParseError( tag, "atomic value expected instead of structure");

			case Value:
			{
				const std::string value( buf, bufpos);
				*((T*)obj) = boost::lexical_cast<T>( value);
			}
			case CloseTag:
				*((T*)obj) = boost::lexical_cast<T>( "");
				throw EndTag();
		}
	}
	else
	{
		throw ParseError( tag, "unexpected end of document");
	}
}

template <typename T>
void parse_( const char* tag, void* obj, vector_&, protocol::InputFilter& flt)
{
	if (inp->getNext( &typ, buf, bufsize-1, &bufpos))
	{
		switch (typ)
		{
			case OpenTag:
			case Attribute:
				throw ParseError( tag, "atomic value expected instead of structure");

			case Value:
			{
				T::value_type val;
				T::value_type::parse_( tag, &val, getCategory(val), flt);
				((T*)obj)->push_back( val);
			}
			case CloseTag:
				T::value_type val;
				((T*)obj)->push_back( val);
				throw EndTag();
		}
	}
	else
	{
		throw ParseError( tag, "unexpected end of document");
	}
}

template <typename T>
void parse_( const char* tag, void* obj, bool_&, protocol::InputFilter& flt)
{
	ElementType typ;
	enum {bufsize=4096};
	char buf[ bufsize];
	std::size_t bufpos;

	if (inp->getNext( &typ, buf, bufsize-1, &bufpos))
	{
		switch (typ)
		{
			case OpenTag:
			case Attribute:
				throw ParseError( tag, "atomic value expected instead of structure");

			case Value:
			{
				const std::string value( buf, bufpos);
				if (boost::algorithm::iequals( value, "yes")
				||  boost::algorithm::iequals( value, "y")
				||  boost::algorithm::iequals( value, "t")
				||  boost::algorithm::iequals( value, "true"))
				{
					*((T*)obj) = true;
				}
				else if (boost::algorithm::iequals( value, "no")
				||  boost::algorithm::iequals( value, "n")
				||  boost::algorithm::iequals( value, "f")
				||  boost::algorithm::iequals( value, "false"))
				{
					*((T*)obj) = false;
				}
				else
				{
					throw ParseError( tag, "boolean value expected");
				}
			}
			case CloseTag:
				throw ParseError( tag, "boolean value expected");
		}
	}
	else
	{
		throw ParseError( tag, "unexpected end of document");
	}
}
}//anonymous namespace

namespace _Wolframe {
namespace serialize {

template <typename T>
static void parse( const char* tag, T* obj, protocol::InputFilter& flt)
{
	try
	{
		parse_<T>( tag, (void*)obj, getCategory(val), flt);
	}
	catch (ParseError& e)
	{
		std::string msg( "error at ");
		msg.append( m_location);
		msg.append( ": ");
		msg.append( m_message);
		throw std::runtime_error( msg.c_str());
	}
}

}}//namespace
#endif

