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
static bool parseObject( const char* tag, ddl::StructType& st, protocol::InputFilter& inp, Context& ctx, bool isinit);


static bool parseAtom( const char* tag, ddl::AtomicType& val, protocol::InputFilter& inp, Context& ctx)
{
	protocol::InputFilter::ElementType typ;
	std::size_t bufpos=0;

	if (inp.getNext( &typ, ctx.buf(), Context::bufsize-1, &bufpos))
	{
		ctx.buf()[ bufpos] = 0;
		switch (typ)
		{
			case protocol::InputFilter::OpenTag:
				ctx.setError( tag, "content value expected instead of tag");
				return false;

			case protocol::InputFilter::Attribute:
				ctx.setError( tag, "content value expected instead of attribute");
				return false;

			case protocol::InputFilter::Value:
				if (!val.set( std::string( ctx.buf())))
				{
					ctx.setError( tag, "illegal value");
					return false;
				}
				break;

			case protocol::InputFilter::CloseTag:
				if (!val.set( std::string( "")))
				{
					ctx.setError( tag, "cannot convert empty string to value");
					return false;
				}
				ctx.endTagConsumed(true);
				break;
		}
	}
	else
	{
		ctx.setError( tag, "unexpected end of content");
		return false;
	}
	return true;
}

static bool parseStruct( const char* tag, ddl::StructType& st, protocol::InputFilter& inp, Context& ctx)
{
	protocol::InputFilter::ElementType typ;
	unsigned int depth = 0;
	std::size_t bufpos = 0;
	std::vector<bool> isinitar;

	while (inp.getNext( &typ, ctx.buf(), Context::bufsize-1, &bufpos))
	{
		ctx.buf()[ bufpos] = 0;
		switch (typ)
		{
			case protocol::InputFilter::OpenTag:
			{
				++depth;
				ddl::StructType::Map::iterator itr = st.find( ctx.buf());
				if (itr == st.end())
				{
					ctx.setError( ctx.buf(), "unknown element");
					ctx.setError( tag);
					return false;
				}
				if (!parseObject( 0, itr->second, inp, ctx, isinitar[ itr-st.begin()])) return false;
				isinitar[ itr-st.begin()] = true;

				if (ctx.endTagConsumed())
				{
					--depth;
					ctx.endTagConsumed(false);
					return true;
				}
				break;
			}

			case protocol::InputFilter::Attribute:
			{
				ddl::StructType::Map::iterator itr = st.find( ctx.buf());
				if (itr == st.end())
				{
					ctx.setError( ctx.buf(), "unknown element");
					ctx.setError( tag);
					return false;
				}
				if ((std::size_t)(itr-st.begin()) >= st.nof_attributes())
				{
					ctx.setError( ctx.buf(), "content element expected");
				}
				if (isinitar[ itr-st.begin()])
				{
					ctx.setError( tag, "duplicate value definition");
					return false;
				}
				try
				{
					if (!parseAtom( ctx.buf(), itr->second.value(), inp, ctx)) return false;
				}
				catch (const std::logic_error&)
				{
					ctx.setError( tag, "Attribute is not atomic");
				}
				isinitar[ itr-st.begin()] = true;

				ctx.endTagConsumed(false);
				break;
			}
			case protocol::InputFilter::Value:
			{
				std::size_t ii;
				for (ii=0; ii<bufpos; ii++) if (ctx.buf()[ii]>32) break;
				if (ii==bufpos) break;
				ctx.endTagConsumed(false);
				ctx.setError( tag, "structure expected");
				return false;
			}
			case protocol::InputFilter::CloseTag:
				if (depth == 0)
				{
					ctx.endTagConsumed(true);
					return true;
				}
				--depth;
				ctx.endTagConsumed(false);
		}
		bufpos = 0;
	}
	return true;
}

static bool parseObject( const char* tag, ddl::StructType& st, protocol::InputFilter& inp, Context& ctx, bool isinit)
{
	bool rt = false;
	switch (st.contentType())
	{
		case ddl::StructType::Atomic:
			if (isinit)
			{
				ctx.setError( tag, "duplicate value definition");
				return false;
			}
			rt = parseAtom( tag, st.value(), inp, ctx);

		case ddl::StructType::Vector:
			st.push();
			rt = parseObject( tag, st.back(), inp, ctx, false);

		case ddl::StructType::Struct:
			if (isinit)
			{
				ctx.setError( tag, "duplicate structure definition");
				return false;
			}
			rt = parseStruct( tag, st, inp, ctx);
	}
	return rt;
}

bool printObject( const char* tag, const ddl::StructType& st, protocol::FormatOutput*& out, Context& ctx)
{
	switch (st.contentType())
	{
		case ddl::StructType::Atomic:
		{
			if (tag && !ctx.printElem( protocol::FormatOutput::OpenTag, tag, std::strlen(tag), out)) return false;
			if (!ctx.printElem( protocol::FormatOutput::Value, st.value().value().c_str(), st.value().value().size(), out)) return false;
			if (tag && !ctx.printElem( protocol::FormatOutput::CloseTag, "", 0, out)) return false;
		}
		case ddl::StructType::Vector:
		{
			ddl::StructType::Map::const_iterator itr=st.begin(), end=st.end();
			for (; itr != end; ++itr)
			{
				if (!printObject( tag, itr->second, out, ctx)) return false;
			}
		}
		case ddl::StructType::Struct:
		{
			bool isContent = false;
			if (tag && !ctx.printElem( protocol::FormatOutput::OpenTag, tag, std::strlen(tag), out)) return false;

			ddl::StructType::Map::const_iterator itr=st.begin(), end=st.end();
			for (std::size_t idx=0; itr != end; ++itr,++idx)
			{
				if (!isContent && itr->second.contentType() == ddl::StructType::Atomic && idx >= st.nof_attributes())
				{
					if (!ctx.printElem( protocol::FormatOutput::Attribute, itr->first.c_str(), itr->first.size(), out)) return false;
					if (!ctx.printElem( protocol::FormatOutput::Value, itr->second.value().value().c_str(), itr->second.value().value().size(), out)) return false;
				}
				else
				{
					isContent = true;
					if (!printObject( itr->first.c_str(), itr->second, out, ctx)) return false;
				}
			}
			if (tag && !ctx.printElem( protocol::FormatOutput::CloseTag, "", 0, out)) return false;
		}
	}
	return true;
}


bool _Wolframe::serialize::parse( ddl::StructType& st, protocol::InputFilter& flt, Context& ctx)
{
	return parseObject( 0, st, flt, ctx, false);
}

bool _Wolframe::serialize::print( const ddl::StructType& st, protocol::FormatOutput*& out, Context& ctx)
{
	return printObject( 0, st, out, ctx);
}




