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
///\file serialize/struct/filtermapBase.cpp
///\brief Implements the non intrusive base class of serialization/deserialization of filters
#include "serialize/struct/filtermapBase.hpp"
#include "filter/typingfilter.hpp"
#include <cstring>
#include <stdexcept>

using namespace _Wolframe;
using namespace serialize;

static std::string getParsePath( const FiltermapParseStateStack& stk)
{
	std::string rt;
	FiltermapParseStateStack::const_iterator itr=stk.begin(), end=stk.end();
	for (; itr != end; ++itr)
	{
		if (itr->name())
		{
			rt.append( "/");
			rt.append( itr->name());
		}
	}
	return rt;
}

static std::string getPrintPath( const FiltermapPrintStateStack& stk)
{
	std::string rt;
	FiltermapPrintStateStack::const_iterator itr=stk.begin(), end=stk.end();
	for (; itr != end; ++itr)
	{
		if (itr->name())
		{
			rt.append( "/");
			rt.append( itr->name());
		}
	}
	return rt;
}

bool FiltermapDescriptionBase::parse( void* obj, langbind::TypedInputFilter& tin, Context& ctx, FiltermapParseStateStack& stk) const
{
	bool rt = true;
	try
	{
		if (stk.size() == 0)
		{
			if (!m_parse) throw std::runtime_error( "null parser called");
			stk.push_back( FiltermapParseState( 0, m_parse, obj));
		}
		while (rt && stk.size())
		{
			rt = stk.back().parse()( tin, ctx, stk);
		}
		if (tin.state() == langbind::InputFilter::Open && !ctx.getLastError() && stk.size() == 1)
		{
			return true;
		}
		if (!rt && ctx.getLastError())
		{
			std::string path = getParsePath( stk);
			ctx.setTag( path.c_str());
		}
	}
	catch (std::exception& e)
	{
		ctx.setError( e.what());
		rt = false;
	}
	return rt;
}

bool FiltermapDescriptionBase::print( const void* obj, langbind::TypedOutputFilter& tout, Context& ctx, FiltermapPrintStateStack& stk) const
{
	bool rt = true;
	try
	{
		if (stk.size() == 0)
		{
			if (!m_print) throw std::runtime_error( "null printer called");
			stk.push_back( FiltermapPrintState( 0, m_print, obj));
		}
		while (rt && stk.size())
		{
			rt = stk.back().print()( tout, ctx, stk);
		}
		if (tout.state() == langbind::OutputFilter::Open && !ctx.getLastError() && stk.size() == 1)
		{
			return true;
		}
		if (!rt && ctx.getLastError())
		{
			std::string path = getPrintPath( stk);
			ctx.setTag( path.c_str());
		}
	}
	catch (std::exception& e)
	{
		ctx.setError( e.what());
		rt = false;
	}
	return rt;
}

