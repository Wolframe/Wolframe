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
///\file serialize/struct/luamapBase.cpp
///\brief Implements the non intrusive base class of serialization for the lua map
#include "serialize/struct/luamapBase.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace serialize;

static int luaException( lua_State* ls)
{
	const char* errmsg = lua_tostring( ls, -1);
	throw std::runtime_error( errmsg?errmsg:"unspecified lua exception");
	return 0;
}

bool LuamapDescriptionBase::parse( void* obj, lua_State* ls, Context* ctx) const
{
	bool gotError = false;
	lua_pushnil( ls);
	lua_pushvalue( ls, -2);
	lua_CFunction old_panicf = lua_atpanic( ls, luaException);
	try
	{
		gotError = !m_parse( obj, ls, ctx);
	}
	catch (std::exception& e)
	{
		ctx->setError( e.what());
		gotError = true;
	}
	lua_atpanic( ls, old_panicf);
	lua_pop( ls, 2);
	return !gotError;
}

bool LuamapDescriptionBase::print( const void* obj, lua_State* ls, Context* ctx) const
{
	bool gotError = false;
	lua_CFunction old_panicf = lua_atpanic( ls, luaException);
	try
	{
		gotError = !m_print(obj,ls, ctx);
	}
	catch (std::exception& e)
	{
		ctx->setError( e.what());
		gotError = true;
	}
	lua_atpanic( ls, old_panicf);
	return !gotError;
}

