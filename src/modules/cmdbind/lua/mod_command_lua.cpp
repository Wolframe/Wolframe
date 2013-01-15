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
///\file mod_command_lua.cpp
///\brief Module for command handler executing lua scripts
#include "module/scriptCommandHandlerBuilder.hpp"
#include "luaCommandHandler.hpp"
#include "logger-v1.hpp"

_Wolframe::log::LogBackend* logBackendPtr;

using namespace _Wolframe;
using namespace _Wolframe::module;

static void setModuleLogger( void* logger)
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend*>( logger);
}

namespace {
static ConfiguredBuilder* createLuaCommandHandlerModule()
{
	static ScriptCommandHandlerBuilder<cmdbind::LuaCommandHandler>
		mod( "LuaCommandHandler", "command handler for lua scripts", "cmdhandler", "lua", "LuaCommandHandler");
	return &mod;
}
}//anonymous namespace

enum {NofContainers=1};
static ConfiguredBuilder* (*containers[ NofContainers])() =
{
	createLuaCommandHandlerModule
};

ModuleEntryPoint entryPoint( 0, "command handler for lua scripts",
				setModuleLogger, NofContainers, containers, 0, 0);

