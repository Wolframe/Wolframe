/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
#include "appdevel/commandHandlerModuleMacros.hpp"
#include "luaCommandHandler.hpp"
#include "luaCommandHandlerConfig.hpp"
#include "luaFunctionProgramType.hpp"

COMMAND_HANDLER_MODULE_WITH_PROGRAMS(
	/*DESCRIPTION: */	"command handler and form function program interpreter for lua",
	/*LANGNAME:*/		Lua,
	/*CONFIG_SECTION:*/	"cmdhandler",
	/*CONFIG_TITLE*/	"lua",
	/*CLASSDEF:*/		_Wolframe::cmdbind::LuaCommandHandler,
	/*CONFIGDEF:*/		_Wolframe::module::LuaCommandHandlerConfig,
	/*CREATEPRGFUNC:*/	_Wolframe::langbind::createLuaProgramType)

