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
///\file serialize/directmapPrint.hpp
///\brief Defines the intrusive implementation of the printing part of serialization for the direct map

#ifndef _Wolframe_DIRECTMAP_PRINT_HPP_INCLUDED
#define _Wolframe_DIRECTMAP_PRINT_HPP_INCLUDED
#include "serialize/directmapTraits.hpp"
#include "serialize/directmapBase.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

namespace _Wolframe {
namespace serialize {

static bool printElem( protocol::FormatOutput::ElementType tp, const void* elem, std::size_t elemsize, protocol::FormatOutput*& out, Context& ctx)
{
	if (!out->print( tp, elem, elemsize))
	{
		ctx.append( out->charptr(), out->pos());
		out->release();
		if (!out->print( tp, elem, elemsize))
		{
			protocol::FormatOutput* ff = out->createFollow();
			if (ff)
			{
				delete out;
				out = ff;
			}
			if (!out->print( tp, elem, elemsize))
			{
				ctx.setError( "buffer of format output too small to hold one element");
				return false;
			}
		}
	}
	return true;
}

template <typename T>
static bool printObject( const char* tag, const T& obj, protocol::FormatOutput*& out, Context& ctx);


template <typename T>
bool print_( const char* tag, const void* obj, const struct_&, protocol::FormatOutput*& out, Context& ctx)
{
	static const DescriptionBase* descr = T::getDescription();
	bool isContent = true;

	if (tag && !printElem( protocol::FormatOutput::OpenTag, tag, std::strlen(tag), out, ctx)) return false;

	DescriptionBase::Map::const_iterator itr = descr->begin(),end = descr->end();
	for (;itr != end; ++itr)
	{
		if (itr->second.isAtomic() && !isContent)
		{
			if (!printElem( protocol::FormatOutput::Attribute, itr->first, std::strlen(itr->first), out, ctx)) return false;
			if (!itr->second.print()( 0, (char*)obj+itr->second.ofs(), out, ctx)) return false;
		}
		else
		{
			isContent = true;
			if (!itr->second.print()( itr->first, (char*)obj+itr->second.ofs(), out, ctx)) return false;
		}
	}
	if (tag && !printElem( protocol::FormatOutput::CloseTag, "", 0, out, ctx)) return false;
	return true;
}

template <typename T>
bool print_( const char* tag, const void* obj, const arithmetic_&, protocol::FormatOutput*& out, Context& ctx)
{
	if (tag && !printElem( protocol::FormatOutput::OpenTag, tag, std::strlen(tag), out, ctx)) return false;
	std::string value( boost::lexical_cast<std::string>( *((T*)obj)));
	if (!printElem( protocol::FormatOutput::Value, value.c_str(), value.size(), out, ctx)) return false;
	if (tag && !printElem( protocol::FormatOutput::CloseTag, "", 0, out, ctx)) return false;
	return true;
}

template <typename T>
bool print_( const char* tag, const void* obj, const bool_&, protocol::FormatOutput*& out, Context& ctx)
{
	if (tag && !printElem( protocol::FormatOutput::OpenTag, tag, std::strlen(tag), out, ctx)) return false;
	if (!printElem( protocol::FormatOutput::Value, (*((T*)obj))?"t":"f", 1, out, ctx)) return false;
	if (tag && !printElem( protocol::FormatOutput::CloseTag, "", 0, out, ctx)) return false;
	return true;
}

template <typename T>
bool print_( const char* tag, const void* obj, const vector_&, protocol::FormatOutput*& out, Context& ctx)
{
	if (!tag)
	{
		ctx.setError( 0, "non printable structure");
		return false;
	}
	for (typename T::const_iterator itr=((T*)obj)->begin(); itr!=((T*)obj)->end(); itr++)
	{
		printElem( protocol::FormatOutput::OpenTag, tag, strlen(tag), out, ctx);
		if (!printObject<typename T::value_type>( 0, *itr, out, ctx)) return false;
		printElem( protocol::FormatOutput::CloseTag, "", 0, out, ctx);
	}
	return true;
}

template <typename T>
static bool printObject( const char* tag, const T& obj, protocol::FormatOutput*& out, Context& ctx)
{
	return print_<T>( tag, (void*)&obj, getCategory(obj), out, ctx);
}

template <typename T>
struct IntrusivePrinter
{
	static bool print( const char* tag, const void* obj, protocol::FormatOutput*& out, Context& ctx)
	{
		if (!print_<T>( tag, obj, getCategory(*(T*)obj), out, ctx)) return false;
		ctx.append( out->charptr(), out->pos());
		out->release();
		return true;
	}
};

}}//namespace
#endif

