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
///\file serialize_ddl_filtermapPrint.cpp

#include "serialize/ddl/filtermapDDLPrint.hpp"
#include "filter/typedfilter.hpp"
#include <cstring>
#include <sstream>

using namespace _Wolframe;
using namespace serialize;

// forward declaration
static bool printObject( langbind::TypedOutputFilter& out, Context& ctx, std::vector<FiltermapDDLPrintState>& stk);

static std::string getPrintPath( const FiltermapDDLPrintStateStack& stk)
{
	std::string rt;
	FiltermapDDLPrintStateStack::const_iterator itr=stk.begin(), end=stk.end();
	for (; itr != end; ++itr)
	{
		if (itr->value() && itr->value()->contentType() != ddl::StructType::Vector)
		{
			std::string tag = itr->tag().tostring();
			if (tag.size())
			{
				rt.append( "/");
				rt.append( tag);
			}
		}
	}
	return rt;
}

static bool printAtom( langbind::TypedOutputFilter& outp, Context& ctx, std::vector<FiltermapDDLPrintState>& stk)
{
	const ddl::AtomicType* val = &stk.back().value()->value();
	std::string ee;
	if (!val->get( ee))
	{
		ctx.setError( "value conversion error");
		return false;
	}
	if (outp.print( langbind::FilterBase::Value, langbind::TypedFilterBase::Element( ee.c_str(), ee.size())))
	{
		stk.pop_back();
		return true;
	}
	else
	{
		return false;
	}
}

static bool printStruct( langbind::TypedOutputFilter& out, Context& ctx, std::vector<FiltermapDDLPrintState>& stk)
{
	const ddl::StructType* obj = (const ddl::StructType*)stk.back().value();
	std::size_t idx = stk.back().state();
	if (idx < obj->nof_elements())
	{
		ddl::StructType::Map::const_iterator itr = obj->begin() + idx;
		if (idx < obj->nof_attributes())
		{
			if (itr->second.contentType() != ddl::StructType::Atomic)
			{
				ctx.setError( "atomic value expected for attribute");
				return false;
			}
			langbind::TypedFilterBase::Element elem( itr->first.c_str(), itr->first.size());
			if (!out.print( langbind::FilterBase::Attribute, elem))
			{
				ctx.setError( out.getError());
				return false;
			}
			stk.push_back( FiltermapDDLPrintState( &itr->second, elem));
			stk.back().state( ++idx);
		}
		else
		{
			langbind::TypedFilterBase::Element elem( itr->first.c_str(), itr->first.size());
			if (!out.print( langbind::FilterBase::OpenTag, elem))
			{
				ctx.setError( out.getError());
				return false;
			}

			stk.back().state( ++idx);
			stk.push_back( FiltermapDDLPrintState( langbind::FilterBase::CloseTag, elem));
			stk.push_back( FiltermapDDLPrintState( &itr->second, elem));
		}
	}
	else
	{
		stk.pop_back();
	}
	return true;
}

static bool printVector( langbind::TypedOutputFilter& out, Context& ctx, std::vector<FiltermapDDLPrintState>& stk)
{
	const ddl::StructType* obj = (const ddl::StructType*)stk.back().value();
	std::size_t idx = stk.back().state();
	if (idx >= obj->nof_elements())
	{
		stk.pop_back();
		return true;
	}
	ddl::StructType::Map::const_iterator itr = obj->begin()+idx;
	if (idx >= 1)
	{
		if (!out.print( langbind::FilterBase::CloseTag, stk.back().tag()))
		{
			ctx.setError( out.getError());
			return false;
		}
	}
	stk.back().state( idx+1);
	stk.push_back( FiltermapDDLPrintState( &itr->second, stk.back().tag()));	//... print element
	if (idx >= 1)
	{
		stk.push_back( FiltermapDDLPrintState( langbind::FilterBase::OpenTag, stk.back().tag()));
	}
	return true;
}

static bool printObject( langbind::TypedOutputFilter& out, Context& ctx, std::vector<FiltermapDDLPrintState>& stk)
{
	if (!stk.back().value())
	{
		if (!out.print( stk.back().type(), stk.back().tag()))
		{
			ctx.setError( out.getError());
			return false;
		}
		stk.pop_back();
		return true;
	}
	switch (stk.back().value()->contentType())
	{
		case ddl::StructType::Atomic:
		{
			return printAtom( out, ctx, stk);
		}
		case ddl::StructType::Vector:
		{
			return printVector( out, ctx, stk);
		}
		case ddl::StructType::Struct:
		{
			return printStruct( out, ctx, stk);
		}
	}
	ctx.setError( "illegal state in print DDL form");
	return false;
}

bool _Wolframe::serialize::print( const ddl::StructType& st, langbind::TypedOutputFilter& tout, Context& ctx, std::vector<FiltermapDDLPrintState>& stk)
{
	langbind::TypedFilterBase::Element elem;
	bool rt = true;
	try
	{
		if (stk.size() == 0)
		{
			stk.push_back( FiltermapDDLPrintState( &st, elem));
		}
		while (rt && stk.size())
		{
			rt = printObject( tout, ctx, stk);
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




