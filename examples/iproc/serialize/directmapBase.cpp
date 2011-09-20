ProcessingContext/************************************************************************
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
#include "directmapBase.hpp"
#include <cstring>

using namespace _Wolframe;
using namespace serialize;
using namespace dm;

bool DescriptionBase::parse( void* obj, protocol::InputFilter& in, ProcessingContext& ctx)
{
	protocol::InputFilter* inp = 0;
	try
	{
		ctx.m_content.append( in.charptr(), in.pos());
		if (!in.gotEoD())
		{
			setState( protocol::InputFilter::EndOfMessage);
			return false;
		}
		inp = in.copy();
		inp->protocolInput( ctx.m_content.c_str(), ctx.m_content.size(), true);
		m_parse( 0, obj, *inp);
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

bool DescriptionBase::print( void* obj, protocol::FormatOutput& out, ProcessingContext& ctx)
{
	try
	{
		m_print( 0, obj, out, ctx.m_content);
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

