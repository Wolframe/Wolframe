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
///\file luaCommandHandlerBuilder.hpp
///\brief Interface of the lua command handler configuration
#ifndef _Wolframe_LUA_COMMAND_HANDLER_CONFIG_HPP_INCLUDED
#define _Wolframe_LUA_COMMAND_HANDLER_CONFIG_HPP_INCLUDED
#include "luaCommandHandler.hpp"
#include "luaScriptContext.hpp"
#include "cmdbind/commandHandler.hpp"
#include "cmdbind/ioFilterCommandHandler.hpp"
#include "config/configurationTree.hpp"
#include "config/configurationBase.hpp"
#include <string>
#include <map>

namespace _Wolframe {
namespace module {

///\brief Named configuration definition based on a langbind::ScriptEnvironmentConfigStruct structure
class LuaCommandHandlerConfig
	:public config::NamedConfiguration
{
public:
	LuaCommandHandlerConfig( const char* classname_, const char* name, const char* logParent, const char* logName)
		:config::NamedConfiguration( name, logParent, logName)
		,m_modules(0)
		,m_classname(classname_){}
	virtual ~LuaCommandHandlerConfig(){}

	///\brief Parse the configuration
	///\param[in] pt configuration tree
	///\param[in] modules module directory
	virtual bool parse( const config::ConfigurationNode& pt, const std::string&, const module::ModulesDirectory* modules);

	///\brief Set canonical path for files referenced as relative path in configuration
	///\param[in] referencePath reference path
	virtual void setCanonicalPathes( const std::string& referencePath);

	virtual bool check() const;

	virtual void print( std::ostream& os, size_t indent ) const;

	const langbind::LuaScriptContext* context() const
	{
		return &m_context;
	}

	virtual const char* className() const
	{
		return m_classname;
	}

	std::list<std::string> commands() const
	{
		return m_context.commands();
	}

private:
	std::vector<std::string> m_programfiles;
	const module::ModulesDirectory* m_modules;
	langbind::LuaScriptContext m_context;
	const char* m_classname;
};

}}//namespace

#endif

