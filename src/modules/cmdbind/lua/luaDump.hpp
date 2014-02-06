/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file luaDump.hpp
///\brief Interface for a dumper/loader of an untouched (just loaded without execution) lua state
#ifndef _WOLFRAME_LUA_DUMP_HPP_INCLUDED
#define _WOLFRAME_LUA_DUMP_HPP_INCLUDED
#include <string>

extern "C" {
#include "lua.h"
}

namespace _Wolframe {
namespace langbind {

//\brief Dump structure
class LuaDump;

//\brief Create a dump of the lua state
//\param[in] ls Lua state to dump
//\return dump or 0 in case of bad_alloc
LuaDump* luaCreateDump( lua_State *ls);

//\brief Load a Lua state dump into a Lua state
//\param[in] ls Lua state to fill with dump
//\param[in] source Lua state dump to load
void luaLoadDump( lua_State *ls, const LuaDump* source);

//\brief Free a Lua dump structure
void freeLuaDump( LuaDump* d);

}}//namespace
#endif


