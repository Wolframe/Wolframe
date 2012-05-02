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
#include "langbind/appObjects.hpp"
#include "langbind/ioFilterCommandHandler.hpp"

namespace _Wolframe {
namespace langbind {

///\brief Create the context for executing a Lua for the Lua command handler. Used for executing a script outside of the command handler in the same way
///\param[in] name of the command to execute, referencing the script
///\param[in] input_ input definition for the input to process
///\param[in] output_ output definition for the output to print
///\return instance of  Lua script with all basic system objects defined
LuaScriptInstanceR createLuaScriptInstance( const char* name, const Input& input_, const Output& output_);



///\class LuaCommandHandler
///\brief command handler instance for processing a call as Lua script
class LuaCommandHandler :public protocol::IOFilterCommandHandler
{
public:
	///\class Context
	///\brief Execution context of the command handler
	struct InterpreterContext;

	///\brief Constructor
	LuaCommandHandler(){}
	///\brief Destructor
	virtual ~LuaCommandHandler(){}

	///\brief Execute the Lua script
	///\param[out] err error code in case of error
	///\return CallResult status (See protocol::IOFilterCommandHandler::CallResult)
	virtual CallResult call( const char*& err);

private:
	LuaScriptInstanceR m_interp;
};

}}//namespace
#endif

