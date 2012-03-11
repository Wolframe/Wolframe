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
///\file luaCommandHandler.hpp
///\brief interface to the lua command handler
#ifndef _Wolframe_langbind_LUA_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_langbind_LUA_COMMAND_HANDLER_HPP_INCLUDED
#include "luaCommandEnvironment.hpp"
#include "appObjects.hpp"
#include "ioFilterCommandHandler.hpp"
extern "C"
{
#include "lua.h"
}

namespace _Wolframe {
namespace langbind {

///\class LuaCommandHandler
///\brief command handler instance for processing a call as Lua script
class LuaCommandHandler :public protocol::IOFilterCommandHandler
{
public:
	///\class Context
	///\brief Execution context of the command handler
	struct Context;

	///\brief Constructor
	///\param[in] config read only reference to the configuration of this application processor
	explicit LuaCommandHandler( const LuaCommandEnvironment* env);
	///\brief Destructor
	virtual ~LuaCommandHandler();

	///\brief Execute the Lua script
	///\param[out] err error code in case of error
	///\return CallResult status (See protocol::IOFilterCommandHandler::CallResult)
	virtual CallResult call( const char*& err);

	///\brief Get the current lua state (not the thread!)
	///\return the current lua state
	lua_State* getLuaState() const;

private:
	const LuaCommandEnvironment* m_env;	///< reference to static environment
	struct Globals
	{
		Input m_input;
		Output m_output;
	};
	Globals m_globals;
	Context* m_context;			///< execution context of the command handler
};

}}//namespace
#endif

