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
///\file serialize/luamapError.hpp
///\brief Defines the error handling of the intrusive lua serialization/deserialization

#ifndef _Wolframe_LUAMAP_ERROR_HPP_INCLUDED
#define _Wolframe_LUAMAP_ERROR_HPP_INCLUDED
#include <cstring>

namespace _Wolframe {
namespace serialize {

struct Context
{
	char errormessage[256];
	char tag[256];

	Context()
	{
		errormessage[ 0] = 0;
		tag[ 0] = 0;
	}

	void setError( const char* tt, const char* ee)
	{
		std::size_t nn = strlen(ee);
		if (nn >= sizeof( errormessage)) nn = sizeof( errormessage)-1;
		std::memcpy( errormessage, ee, nn);
		errormessage[ nn] = 0;
		if (tt)
		{
			nn = strlen(tt);
			if (nn >= sizeof( tag)) nn = sizeof( tag)-1;
			std::memcpy( tag, tt, nn);
			tag[ nn] = 0;
		}
		else
		{
			tag[ 0] = 0;
		}
	}

	void setError( const char* tt, Context* ctx)
	{
		if (!tt) return;
		char buf[ sizeof(tag)];
		std::size_t nn = std::strlen(tt);
		if (nn >= sizeof( buf)) nn = sizeof( buf)-2;
		std::memcpy( buf, tt, nn);
		buf[ nn] = '/';
		buf[ ++nn] = 0;
		std::size_t mm = std::strlen(ctx->tag);
		if (mm+nn >= sizeof( buf)) mm = sizeof( buf)-1-nn;
		std::memcpy( buf+nn, ctx->tag, mm);
		buf[ nn+mm] = 0;
		std::memcpy( tag, buf, nn+mm+1);
	}
};

}}
#endif

