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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file luaCommandHandler.hpp
///\brief Interface to the lua command handler
#ifndef _Wolframe_cmdbind_LUA_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_cmdbind_LUA_COMMAND_HANDLER_HPP_INCLUDED
#include "cmdbind/ioFilterCommandHandlerEscDLF.hpp"
#include "luaObjects.hpp"
#include "luaScriptContext.hpp"
#include <vector>
#include <string>

namespace _Wolframe {
namespace cmdbind {

///\class LuaCommandHandler
///\brief command handler instance for processing a call as Lua script
class LuaCommandHandler :public IOFilterCommandHandlerEscDLF
{
public:
	typedef IOFilterCommandHandlerEscDLF Parent;

public:
	///\brief Constructor
	explicit LuaCommandHandler( const langbind::LuaScriptContext* ctx_)
		:m_ctx(ctx_){}

	///\brief Destructor
	virtual ~LuaCommandHandler(){}

	///\brief Execute the Lua script
	///\param[out] err error code in case of error
	///\return CallResult status (See IOFilterCommandHandler::CallResult)
	virtual CallResult call( const char*& err);

	///\brief Get the identifier of this command handler type
	static const char* identifier()
	{
		return "LuaCommandHandler";
	}

private:
	void initcall( const std::string& docformat);

private:
	const langbind::LuaScriptContext* m_ctx;
	langbind::LuaScriptInstanceR m_interp;
	std::string m_lasterror;
};

}}//namespace
#endif

