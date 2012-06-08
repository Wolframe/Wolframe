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
#include "filter/serializefilter.hpp"
#include <cstring>
#include <sstream>

using namespace _Wolframe;
using namespace serialize;

// forward declaration
static bool printObject( langbind::TypedOutputFilter& out, Context& ctx, std::vector<FiltermapDDLPrintState>& stk);

static bool printAtom( const ddl::AtomicType& val, langbind::TypedOutputFilter& outp, Context& ctx, std::vector<FiltermapDDLPrintState>& stk)
{
	std::string ee;
	if (!val.get( ee))
	{
		ctx.setError( "value conversion error");
		return false;
	}
	bool rt = outp.print( langbind::OutputFilter::Value, langbind::TypedFilterBase::Element( ee.c_str(), ee.size()));
	stk.pop_back();
	return rt;
}

static bool printStruct( const ddl::StructType& val, langbind::TypedOutputFilter& out, Context& ctx, std::vector<FiltermapDDLPrintState>& stk)
{
	if (stk.back().state() < val.size()*2)
	{
		std::size_t idx = stk.back().state()/2;
		bool atEnd = stk.back().state()&1;
		ddl::StructType::Map::const_iterator itr = val.begin() + idx;

		if (atEnd)
		{
			if (!out.print( langbind::TypedOutputFilter::CloseTag, langbind::TypedFilterBase::Element( itr->first.c_str(), itr->first.size())))
			{
				ctx.setError( out.getError());
				return false;
			}
			++idx;
			++itr;
			stk.back().state( idx*2);
		}
		if (idx < val.nof_attributes())
		{
			if (itr->second.contentType() != ddl::StructType::Atomic)
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
			stk.push_back( FiltermapDDLPrintState( &itr->second, elem));
			++idx;
			stk.back().state( 2*idx);
		}
		else
		{
			langbind::TypedFilterBase::Element elem( itr->first.c_str(), itr->first.size());
			if (!out.print( langbind::TypedOutputFilter::OpenTag, elem))
			{
				ctx.setError( out.getError());
				return false;
			}
			stk.back().state( 2*idx+1);
			stk.push_back( FiltermapDDLPrintState( &itr->second, elem));
		}
	}
	else
	{
		stk.pop_back();
	}
	return true;
}

static bool printVector( const ddl::StructType& val, langbind::TypedOutputFilter& out, Context& ctx, std::vector<FiltermapDDLPrintState>& stk)
{
	langbind::TypedFilterBase::Element elem;

	std::size_t idx = stk.back().state()/4;
	std::size_t substate = stk.back().state()&3;

	if (idx >= val.size())
	{
		stk.pop_back();
		return true;
	}
	switch (substate)
	{
		case 0:
		{
			ddl::StructType::Map::const_iterator itr = val.begin() + idx;
			stk.push_back( FiltermapDDLPrintState( &itr->second, elem));
			if (idx+1 < val.size())
			{
				stk.back().state( idx*4 +1);
			}
			else
			{
				stk.back().state( ++idx*4);
			}
			return true;
		}
		case 1:
		{
			if (!out.print( langbind::TypedOutputFilter::CloseTag, stk.back().tag()))
			{
				ctx.setError( out.getError());
				return false;
			}
			stk.back().state( idx*4 +2);
		}
		default:
		{
			if (!out.print( langbind::TypedOutputFilter::OpenTag, stk.back().tag()))
			{
				ctx.setError( out.getError());
				return false;
			}
			stk.back().state( ++idx*4);
		}
	}
	return true;
}

static bool printObject( langbind::TypedOutputFilter& out, Context& ctx, std::vector<FiltermapDDLPrintState>& stk)
{
	switch (stk.back().value()->contentType())
	{
		case ddl::StructType::Atomic:
		{
			return printAtom( stk.back().value()->value(), out, ctx, stk);
		}
		case ddl::StructType::Vector:
		{
			return printVector( *stk.back().value(), out, ctx, stk);
		}
		case ddl::StructType::Struct:
		{
			return printStruct( *stk.back().value(), out, ctx, stk);
		}
	}
	return true;
}

bool _Wolframe::serialize::print( const ddl::StructType& st, langbind::OutputFilter& out, Context& ctx, std::vector<FiltermapDDLPrintState>& stk)
{
	langbind::TypedFilterBase::Element elem;
	bool rt = true;
	try
	{
		langbind::SerializeOutputFilter tout( &out);
		if (stk.size() == 0)
		{
			stk.push_back( FiltermapDDLPrintState( &st, elem));
		}
		while (rt && stk.size())
		{
			rt = printObject( tout, ctx, stk);
		}
	}
	catch (std::exception& e)
	{
		ctx.setError( e.what());
		rt = false;
	}
	return rt;
}




