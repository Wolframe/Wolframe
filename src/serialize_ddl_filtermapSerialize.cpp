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
///\file serialize/ddl/filtermapSerialize.cpp

#include "serialize/ddl/filtermapSerialize.hpp"
#include <cstring>

using namespace _Wolframe;
using namespace serialize;

// forward declaration
static bool parseObject( ddl::StructType& st, langbind::InputFilter& inp, Context& ctx, bool isinit);


static bool parseAtom( ddl::AtomicType& val, langbind::InputFilter& inp, Context& ctx)
{
	langbind::InputFilter::ElementType typ;
	const void* element;
	std::size_t elementsize;

	if (inp.getNext( typ, element, elementsize))
	{
		switch (typ)
		{
			case langbind::InputFilter::OpenTag:
				ctx.setError( "content value expected instead of tag");
				return false;

			case langbind::InputFilter::Attribute:
				ctx.setError( "content value expected instead of attribute");
				return false;

			case langbind::InputFilter::Value:
				if (!val.set( std::string( (const char*)element, elementsize)))
				{
					ctx.setError( "illegal value");
					return false;
				}
				break;

			case langbind::InputFilter::CloseTag:
				if (!val.set( std::string( "")))
				{
					ctx.setError( "cannot convert empty string to value");
					return false;
				}
				ctx.followTagConsumed(true);
				break;
		}
	}
	return (inp.state() == langbind::InputFilter::Open);
}

static bool parseStruct( ddl::StructType& st, langbind::InputFilter& inp, Context& ctx)
{
	langbind::InputFilter::ElementType typ;
	unsigned int depth = 0;
	const void* element;
	std::size_t elementsize;
	std::vector<bool> isinitar;

	while (inp.getNext( typ, element, elementsize))
	{
		switch (typ)
		{
			case langbind::InputFilter::OpenTag:
			{
				++depth;
				ddl::StructType::Map::iterator itr = st.find( std::string( (const char*)element, elementsize));
				if (itr == st.end())
				{
					ctx.setError( "unknown element", (const char*)element);
					return false;
				}
				if (!parseObject( itr->second, inp, ctx, isinitar[ itr-st.begin()]))
				{
					ctx.setTag( itr->first.c_str());
					return false;
				}
				isinitar[ itr-st.begin()] = true;

				if (ctx.followTagConsumed())
				{
					--depth;
					ctx.followTagConsumed(false);
					return true;
				}
				break;
			}

			case langbind::InputFilter::Attribute:
			{
				ddl::StructType::Map::iterator itr = st.find( std::string( (const char*)element, elementsize));
				if (itr == st.end())
				{
					ctx.setError( "unknown element", (const char*)element);
					return false;
				}
				if ((std::size_t)(itr-st.begin()) >= st.nof_attributes())
				{
					ctx.setError( "content element expected", (const char*)element);
				}
				if (isinitar[ itr-st.begin()])
				{
					ctx.setError( "duplicate value definition", (const char*)element);
					return false;
				}
				try
				{
					if (!parseAtom( itr->second.value(), inp, ctx))
					{
						ctx.setTag( (const char*)element);
						return false;
					}
				}
				catch (const std::logic_error&)
				{
					ctx.setError( "Attribute is not atomic");
				}
				isinitar[ itr-st.begin()] = true;

				ctx.followTagConsumed(false);
				break;
			}
			case langbind::InputFilter::Value:
			{
				std::size_t ii;
				for (ii=0; ii<elementsize; ii++) if (((const char*)element)[ii]>32) break;
				if (ii==elementsize) break;
				ctx.followTagConsumed(false);
				ctx.setError( "structure expected");
				return false;
			}
			case langbind::InputFilter::CloseTag:
				if (depth == 0)
				{
					ctx.followTagConsumed(true);
					return true;
				}
				--depth;
				ctx.followTagConsumed(false);
		}
	}
	return (inp.state() == langbind::InputFilter::Open);
}

static bool parseObject( ddl::StructType& st, langbind::InputFilter& inp, Context& ctx, bool isinit)
{
	bool rt = false;
	switch (st.contentType())
	{
		case ddl::StructType::Atomic:
			if (isinit)
			{
				ctx.setError( "duplicate value definition");
				return false;
			}
			rt = parseAtom( st.value(), inp, ctx);

		case ddl::StructType::Vector:
			st.push();
			rt = parseObject( st.back(), inp, ctx, false);

		case ddl::StructType::Struct:
			if (isinit)
			{
				ctx.setError( "duplicate structure definition");
				return false;
			}
			rt = parseStruct( st, inp, ctx);
	}
	return rt;
}

bool printObject( const char* tag, const ddl::StructType& st, langbind::OutputFilter& out, Context& ctx)
{
	switch (st.contentType())
	{
		case ddl::StructType::Atomic:
		{
			if (tag && !ctx.printElem( langbind::OutputFilter::OpenTag, tag, std::strlen(tag), out)) return false;
			if (!ctx.printElem( langbind::OutputFilter::Value, st.value().value().c_str(), st.value().value().size(), out)) return false;
			if (tag && !ctx.printElem( langbind::OutputFilter::CloseTag, "", 0, out)) return false;
		}
		case ddl::StructType::Vector:
		{
			ddl::StructType::Map::const_iterator itr=st.begin(), end=st.end();
			for (; itr != end; ++itr)
			{
				if (!printObject( itr->first.c_str(), itr->second, out, ctx)) return false;
			}
		}
		case ddl::StructType::Struct:
		{
			bool isContent = false;
			if (tag && !ctx.printElem( langbind::OutputFilter::OpenTag, tag, std::strlen(tag), out)) return false;

			ddl::StructType::Map::const_iterator itr=st.begin(), end=st.end();
			for (std::size_t idx=0; itr != end; ++itr,++idx)
			{
				if (!isContent && itr->second.contentType() == ddl::StructType::Atomic && idx >= st.nof_attributes())
				{
					if (!ctx.printElem( langbind::OutputFilter::Attribute, itr->first.c_str(), itr->first.size(), out)) return false;
					if (!ctx.printElem( langbind::OutputFilter::Value, itr->second.value().value().c_str(), itr->second.value().value().size(), out)) return false;
				}
				else
				{
					isContent = true;
					if (!printObject( itr->first.c_str(), itr->second, out, ctx)) return false;
				}
			}
			if (tag && !ctx.printElem( langbind::OutputFilter::CloseTag, "", 0, out)) return false;
		}
	}
	return true;
}


bool _Wolframe::serialize::parse( ddl::StructType& st, langbind::BufferingInputFilter& flt, Context& ctx)
{
	return parseObject( st, flt, ctx, false);
}

bool _Wolframe::serialize::print( const ddl::StructType& st, langbind::OutputFilter& out, Context& ctx)
{
	return printObject( (const char*)0, st, out, ctx);
}




