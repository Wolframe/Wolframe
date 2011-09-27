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
///\file serialize/directmapBase.cpp
///\brief Implements the non intrusive base class of serialization for the direct map
#include "serialize/directmapBase.hpp"
#include <cstring>

using namespace _Wolframe;
using namespace serialize;

bool DescriptionBase::parse( const char* name, void* obj, protocol::InputFilter& in, ProcessingContext& ctx) const
{
	protocol::InputFilter* inp = 0;
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
		char tagbuf[ 1024];
		std::size_t tagpos = 0;
		protocol::InputFilter::ElementType etyp;
		while (!inp->getNext( &etyp, tagbuf, sizeof(tagbuf)-1, &tagpos))
		{
			protocol::InputFilter* ff = inp->createFollow();
			if (!ff)
			{
				throw std::logic_error( "failed to parse xml header");
			}
			else
			{
				delete inp;
				inp = ff;
			}
		}
		tagbuf[ tagpos] = 0;
		if (etyp != protocol::InputFilter::OpenTag)
		{
			throw std::logic_error( "failed to parse xml root element");
		}
		if (std::strcmp(name, tagbuf) != 0)
		{
			throw std::logic_error( "xml is of different type than expected");
		}
		if (m_parse)
		{
			m_parse( 0, obj, *inp, ctx);
		}
		else
		{
			throw std::logic_error( "null parser called");
		}
		if (!ctx.endTagConsumed())
		{
			if (!inp->getNext( &etyp, tagbuf, sizeof(tagbuf), &tagpos) || etyp != protocol::InputFilter::CloseTag)
			{
				throw std::logic_error( "xml not properly balanced or illegal");
			}
		}
		ctx.m_content.clear();
		delete inp;
	}
	catch (std::exception& e)
	{
		const char* msg = e.what();
		std::size_t msgsize = std::strlen( msg);
		if (msgsize > sizeof( ctx.m_lasterror))
		{
			msgsize = sizeof( ctx.m_lasterror)-1;
		}
		std::memcpy( ctx.m_lasterror, msg, msgsize);
		ctx.m_lasterror[ msgsize] = 0;
		ctx.m_content.clear();
		delete inp;
		return false;
	}
	return true;
}

bool DescriptionBase::print( const char* name, const void* obj, protocol::FormatOutput& out, ProcessingContext& ctx) const
{
	try
	{
		char outputbuffer[ 8192];
		if (m_print)
		{
			protocol::FormatOutput* oo = out.copy();
			oo->init( (void*)outputbuffer, sizeof(outputbuffer));
			m_print( name, obj, oo, ctx.m_content);
		}
		else
		{
			throw std::logic_error( "null printer called");
		}
	}
	catch (std::exception& e)
	{
		const char* msg = e.what();
		std::size_t msgsize = std::strlen( msg);
		if (msgsize > sizeof( ctx.m_lasterror))
		{
			msgsize = sizeof( ctx.m_lasterror)-1;
		}
		std::memcpy( ctx.m_lasterror, msg, msgsize);
		ctx.m_lasterror[ msgsize] = 0;
		return false;
	}
	return true;
}

