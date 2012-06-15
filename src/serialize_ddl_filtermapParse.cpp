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
///\file serialize_ddl_filtermapParse.cpp

#include "serialize/ddl/filtermapDDLParse.hpp"
#include "filter/serializefilter.hpp"
#include <cstring>
#include <sstream>

using namespace _Wolframe;
using namespace serialize;

// forward declarations
static bool parseObject( langbind::TypedInputFilter& inp, Context& ctx, std::vector<FiltermapDDLParseState>& stk);


static bool parseAtom( ddl::AtomicType& val, langbind::TypedInputFilter& inp, Context& ctx, std::vector<FiltermapDDLParseState>& stk)
{
	langbind::InputFilter::ElementType typ;
	langbind::TypedFilterBase::Element element;

	if (inp.getNext( typ, element))
	{
		switch (typ)
		{
			case langbind::InputFilter::OpenTag:
				ctx.setError( "atomic value expected instead of tag");
				return false;

			case langbind::InputFilter::Attribute:
				ctx.setError( "atomic value expected instead of attribute");
				return false;

			case langbind::InputFilter::Value:
				switch (element.type)
				{
					case langbind::TypedFilterBase::Element::bool_:
						if (!val.set( element.value.bool_))
						{
							ctx.setError( "illegal value");
							return false;
						}
					break;
					case langbind::TypedFilterBase::Element::double_:
						if (!val.set( element.value.double_))
						{
							ctx.setError( "illegal value");
							return false;
						}
					break;
					case langbind::TypedFilterBase::Element::int_:
						if (!val.set( element.value.int_))
						{
							ctx.setError( "illegal value");
							return false;
						}
					break;
					case langbind::TypedFilterBase::Element::uint_:
						if (!val.set( element.value.uint_))
						{
							ctx.setError( "illegal value");
							return false;
						}
					break;
					case langbind::TypedFilterBase::Element::string_:
						if (!val.set( std::string( element.value.string_.ptr, element.value.string_.size)))
						{
							ctx.setError( "illegal value");
							return false;
						}
					break;
				}
				break;

			case langbind::InputFilter::CloseTag:
				if (!val.set( std::string( "")))
				{
					ctx.setError( "cannot convert empty string to value");
					return false;
				}
				stk.pop_back();
				break;
		}
		return true;
	}
	return false;
}

static bool parseStruct( ddl::StructType& st, langbind::TypedInputFilter& inp, Context& ctx, std::vector<FiltermapDDLParseState>& stk)
{
	langbind::InputFilter::ElementType typ;
	langbind::TypedFilterBase::Element element;

	stk.back().initStructDef( st.nof_elements());

	if (!inp.getNext( typ, element))
	{
		const char* err = inp.getError();
		if (err) ctx.setError( err);
		return false;
	}
	switch (typ)
	{
		case langbind::InputFilter::OpenTag:
		{
			ddl::StructType::Map::iterator itr = st.find( element.tostring());
			if (itr == st.end())
			{
				ctx.setError( "unknown element");
				return false;
			}
			std::size_t idx = itr - st.begin();
			if (idx < st.nof_attributes())
			{
				if (ctx.flag( Context::ValidateAttributes))
				{
					ctx.setError( "attribute element expected");
					return false;
				}
			}
			stk.back().selectElement( idx);
			stk.push_back( FiltermapDDLParseState( &itr->second));
			return true;
		}

		case langbind::InputFilter::Attribute:
		{
			ddl::StructType::Map::iterator itr = st.find( element.tostring());
			if (itr == st.end())
			{
				ctx.setError( "unknown element");
				return false;
			}
			std::size_t idx = itr - st.begin();
			if (idx >= st.nof_attributes())
			{
				if (ctx.flag( Context::ValidateAttributes))
				{
					ctx.setError( "content element expected");
					return false;
				}
			}
			if (itr->second.contentType() != ddl::StructType::Atomic)
			{
				ctx.setError( "atomic element expected");
				return false;
			}
			stk.back().selectElement( idx);
			stk.push_back( FiltermapDDLParseState( &itr->second));
			return true;
		}

		case langbind::InputFilter::Value:
		{
			ctx.setError( "structure instead of value expected");
			return false;
		}

		case langbind::InputFilter::CloseTag:
		{
			ddl::StructType::Map::iterator itr = st.begin(), end = st.end();
			for (;itr != end; ++itr)
			{
				if (itr->second.mandatory() && stk.back().initCount( itr-st.begin()) == 0)
				{
					ctx.setError( "undefined mandatory element");
					return false;
				}
			}
			stk.pop_back();
			return true;
		}
	}
	ctx.setError( "illegal state");
	return false;
}


static bool parseObject( langbind::TypedInputFilter& inp, Context& ctx, std::vector<FiltermapDDLParseState>& stk)
{
	bool rt = false;
	switch (stk.back().value()->contentType())
	{
		case ddl::StructType::Atomic:
		{
			if (2>=stk.at( stk.size()-1).initCount())
			{
				ctx.setError( "duplicate value definition");
				return false;
			}
			rt = parseAtom( stk.back().value()->value(), inp, ctx, stk);
		}
		case ddl::StructType::Vector:
		{
			if (stk.back().state())
			{
				stk.pop_back();
				return true;
			}
			stk.back().state( 1);
			stk.back().value()->push();
			stk.push_back( FiltermapDDLParseState( &stk.back().value()->back()));
			return true;
		}
		case ddl::StructType::Struct:
		{
			if (2>=stk.at( stk.size()-1).initCount())
			{
				ctx.setError( "duplicate structure definition");
				return false;
			}
			rt = parseStruct( *stk.back().value(), inp, ctx, stk);
		}
	}
	return rt;
}


bool _Wolframe::serialize::parse( ddl::StructType& st, langbind::TypedInputFilter& tin, Context& ctx, std::vector<FiltermapDDLParseState>& stk)
{
	bool rt = true;
	try
	{
		if (stk.size() == 0)
		{
			stk.push_back( FiltermapDDLParseState( &st));
		}
		while (rt && stk.size())
		{
			rt = parseObject( tin, ctx, stk);
		}
	}
	catch (std::exception& e)
	{
		ctx.setError( e.what());
		return false;
	}
	return rt;
}

