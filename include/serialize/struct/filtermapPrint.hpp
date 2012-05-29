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
///\file serialize/struct/filtermapPrint.hpp
///\brief Defines the intrusive implementation of the printing part of serialization of filters

#ifndef _Wolframe_SERLIALIZE_STRUCT_FILTERMAP_PRINT_HPP_INCLUDED
#define _Wolframe_SERLIALIZE_STRUCT_FILTERMAP_PRINT_HPP_INCLUDED
#include "serialize/struct/filtermapTraits.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

namespace _Wolframe {
namespace serialize {

template <typename T>
static bool printObject( const char* tag, const T& obj, langbind::OutputFilter& out, Context& ctx);

template <typename T>
bool print_( const char* tag, const void* obj, const filtertraits::struct_&, langbind::OutputFilter& out, Context& ctx)
{
	static const FiltermapDescriptionBase* descr = T::getFiltermapDescription();
	bool isContent = true;

	if (tag && !ctx.printElem( langbind::OutputFilter::OpenTag, tag, std::strlen(tag), out)) return false;

	FiltermapDescriptionBase::Map::const_iterator itr = descr->begin(),end = descr->end();
	for (std::size_t idx=0; itr != end; ++itr,++idx)
	{
		if (itr->second.isAtomic() && !isContent && idx < descr->nof_attributes())
		{
			if (!ctx.printElem( langbind::OutputFilter::Attribute, itr->first.c_str(), itr->first.size(), out)
			||  !itr->second.print()( itr->first.c_str(), (char*)obj+itr->second.ofs(), out, ctx))
			{
				if (tag) ctx.setTag( itr->first.c_str());
				return false;
			}
		}
		else
		{
			isContent = true;
			if (!itr->second.print()( itr->first.c_str(), (char*)obj+itr->second.ofs(), out, ctx)) return false;
		}
	}
	if (tag && !ctx.printElem( langbind::OutputFilter::CloseTag, "", 0, out)) return false;
	return true;
}

template <typename T>
bool print_( const char* tag, const void* obj, const filtertraits::arithmetic_&, langbind::OutputFilter& out, Context& ctx)
{
	if (tag && !ctx.printElem( langbind::OutputFilter::OpenTag, tag, std::strlen(tag), out)) return false;
	std::string value( boost::lexical_cast<std::string>( *((T*)obj)));
	if (!ctx.printElem( langbind::OutputFilter::Value, value.c_str(), value.size(), out)) return false;
	if (tag && !ctx.printElem( langbind::OutputFilter::CloseTag, "", 0, out)) return false;
	return true;
}

template <typename T>
bool print_( const char* tag, const void* obj, const filtertraits::bool_&, langbind::OutputFilter& out, Context& ctx)
{
	if (tag && !ctx.printElem( langbind::OutputFilter::OpenTag, tag, std::strlen(tag), out)) return false;
	if (!ctx.printElem( langbind::OutputFilter::Value, (*((T*)obj))?"t":"f", 1, out)) return false;
	if (tag && !ctx.printElem( langbind::OutputFilter::CloseTag, "", 0, out)) return false;
	return true;
}

template <typename T>
bool print_( const char* tag, const void* obj, const filtertraits::vector_&, langbind::OutputFilter& out, Context& ctx)
{
	if (!tag)
	{
		ctx.setError( "non printable structure");
		return false;
	}
	for (typename T::const_iterator itr=((T*)obj)->begin(); itr!=((T*)obj)->end(); itr++)
	{
		ctx.printElem( langbind::OutputFilter::OpenTag, tag, strlen(tag), out);
		if (!printObject<typename T::value_type>( 0, *itr, out, ctx)) return false;
		ctx.printElem( langbind::OutputFilter::CloseTag, "", 0, out);
	}
	return true;
}

template <typename T>
static bool printObject( const char* tag, const T& obj, langbind::OutputFilter& out, Context& ctx)
{
	return print_<T>( tag, (void*)&obj, filtertraits::getCategory(obj), out, ctx);
}

template <typename T>
struct FiltermapIntrusivePrinter
{
	static bool print( const char* tag, const void* obj, langbind::OutputFilter& out, Context& ctx)
	{
		if (!print_<T>( tag, obj, filtertraits::getCategory(*(T*)obj), out, ctx)) return false;
		return true;
	}
};

}}//namespace
#endif

