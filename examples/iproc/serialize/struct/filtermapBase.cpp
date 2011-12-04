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
#include <cstring>

using namespace _Wolframe;
using namespace serialize;

bool DescriptionBase::parse( const char* name, void* obj, protocol::InputFilter& in, Context& ctx) const
{
	protocol::InputFilter* inp = 0;
	bool rt = true;
	try
	{
		ctx.m_content.append( (char*)in.ptr(), in.size());
		in.skip( in.size());

		if (!in.gotEoD())
		{
			in.setState( protocol::InputFilter::EndOfMessage);
			return false;
		}
		in.setState( protocol::InputFilter::Open);
		inp = in.copy();
		inp->protocolInput( (void*)ctx.m_content.c_str(), ctx.m_content.size(), true);

		std::size_t bufpos = 0;
		protocol::InputFilter::ElementType etyp;

		while (!inp->getNext( &etyp, ctx.buf(), ctx.bufsize-1, &bufpos))
		{
			protocol::InputFilter* ff = inp->createFollow();
			if (!ff)
			{
				ctx.setError( 0, "failed to parse xml header");
			}
			else
			{
				delete inp;
				inp = ff;
			}
			bufpos = 0;
		}
		ctx.buf()[ bufpos] = 0;
		if (etyp != protocol::InputFilter::OpenTag)
		{
			ctx.setError( 0, "failed to parse xml root element");
			rt = false;
		}
		else if (std::strcmp(name, ctx.buf()) != 0)
		{
			ctx.setError( 0, "xml is of different type than expected");
			rt = false;
		}
		else if (m_parse)
		{
			rt &= m_parse( 0, obj, *inp, ctx);
		}
		else
		{
			ctx.setError( 0, "null parser called");
			rt = false;
		}
		if (rt && !ctx.endTagConsumed())
		{
			bufpos = 0;
			if (!inp->getNext( &etyp, ctx.buf(), ctx.bufsize-1, &bufpos) || etyp != protocol::InputFilter::CloseTag)
			{
				ctx.setError( 0, "xml not properly balanced or illegal");
				rt = false;
			}
		}
	}
	catch (std::exception& e)
	{
		ctx.setError( 0, e.what());
		rt = false;
	}
	ctx.m_content.clear();
	if (inp) delete inp;
	return rt;
}

bool DescriptionBase::print( const char* name, const void* obj, protocol::FormatOutput& out, Context& ctx) const
{
	try
	{
		if (m_print)
		{
			protocol::FormatOutput* oo = out.copy();
			oo->init( (void*)ctx.buf(), ctx.bufsize);
			return m_print( name, obj, oo, ctx);
		}
		else
		{
			ctx.setError( 0, "null printer called");
			return false;
		}
	}
	catch (std::exception& e)
	{
		ctx.setError( name, e.what());
		return false;
	}
	return true;
}

