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
#include "filter/typedfilter.hpp"
#include "serialize/struct/filtermapTraits.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include "serialize/struct/filtermapPrintStack.hpp"
#include "serialize/struct/filtermapPrintValue.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

namespace _Wolframe {
namespace serialize {

// forward declaration
template <typename T>
struct FiltermapIntrusivePrinter
{
	static bool print( langbind::TypedOutputFilter& out, Context& ctx, FiltermapPrintStateStack& stk);
};

static bool printCloseTag( langbind::TypedOutputFilter& out, Context& ctx, FiltermapPrintStateStack& stk)
{
	if (!out.print( langbind::TypedOutputFilter::CloseTag, langbind::TypedFilterBase::Element()))
	{
		ctx.setError( out.getError());
		return false;
	}
	stk.pop_back();
	return true;
}

template <typename T>
static bool printObject_( const traits::struct_&, langbind::TypedOutputFilter& out, Context& ctx, FiltermapPrintStateStack& stk)
{
	const void* obj = stk.back().value();

	static const FiltermapDescriptionBase* descr = T::getFiltermapDescription();
	std::size_t idx = stk.back().state();
	if (idx < descr->nof_elements())
	{
		FiltermapDescriptionBase::Map::const_iterator itr = descr->begin() + idx;

		if (idx < descr->nof_attributes())
		{
			if (itr->second.type() != FiltermapDescriptionBase::Atomic)
			{
				ctx.setError( "atomic value expected for attribute");
				return false;
			}
			langbind::TypedFilterBase::Element elem( itr->first.c_str(), itr->first.size());
			if (!out.print( langbind::TypedOutputFilter::Attribute, elem))
			{
				ctx.setError( out.getError());
				return false;
			}
			stk.back().state( idx+1);
			stk.push_back( FiltermapPrintState( itr->first.c_str(), itr->second.print(), (char*)obj + itr->second.ofs()));
		}
		else
		{
			langbind::TypedFilterBase::Element elem( itr->first.c_str(), itr->first.size());
			if (!out.print( langbind::TypedOutputFilter::OpenTag, elem))
			{
				ctx.setError( out.getError());
				return false;
			}
			stk.back().state( idx+1);
			stk.push_back( FiltermapPrintState( 0, &printCloseTag, itr->first.c_str()));
			stk.push_back( FiltermapPrintState( itr->first.c_str(), itr->second.print(), (char*)obj + itr->second.ofs()));
		}
	}
	else
	{
		stk.pop_back();
	}
	return true;
}

template <typename T>
static bool printObject_( const traits::atomic_&, langbind::TypedOutputFilter& out, Context& ctx, FiltermapPrintStateStack& stk)
{
	langbind::TypedInputFilter::Element element;
	if (!traits::printValue( *(T*)stk.back().value(), element))
	{
		ctx.setError( "atomic value conversion error");
		return false;
	}
	if (!out.print( langbind::TypedOutputFilter::Value, element))
	{
		ctx.setError( out.getError());
		return false;
	}
	stk.pop_back();
	return true;
}

template <typename T>
static bool printObject_( const traits::vector_&, langbind::TypedOutputFilter& out, Context& ctx, FiltermapPrintStateStack& stk)
{
	typedef typename T::value_type Element;
	std::vector<Element>* obj = (T*)stk.back().value();

	std::size_t idx = stk.back().state()/2;
	std::size_t sdx = stk.back().state() & 1;
	if (idx >= obj->size())
	{
		stk.pop_back();
		return true;
	}
	if (idx >= 1)
	{
		langbind::TypedInputFilter::Element tag( (const char*)stk.at( stk.size()-2).value());
		if (sdx == 0)
		{
			if (!out.print( langbind::TypedOutputFilter::CloseTag, tag))
			{
				ctx.setError( out.getError());
				return false;
			}
		}
		if (!out.print( langbind::TypedOutputFilter::OpenTag, tag))
		{
			stk.back().state( idx*2+1);
			ctx.setError( out.getError());
			return false;
		}
	}
	stk.back().state( (idx+1)*2);
	Element* ve = &(*obj)[ idx];
	stk.push_back( FiltermapPrintState( stk.back().name(), &FiltermapIntrusivePrinter<Element>::print, ve));
	return true;
}

template <typename T>
bool FiltermapIntrusivePrinter<T>::print( langbind::TypedOutputFilter& out, Context& ctx, FiltermapPrintStateStack& stk)
{
	static T* t = 0;
	return printObject_<T>( traits::getFiltermapCategory(*t), out, ctx, stk);
}

}}//namespace
#endif

