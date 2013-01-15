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
///\file langbind/luaScriptContext.hpp
///\brief Objects defined by a command handler processing lua scripts
#ifndef _Wolframe_langbind_LUA_SCRIPT_CONTEXT_HPP_INCLUDED
#define _Wolframe_langbind_LUA_SCRIPT_CONTEXT_HPP_INCLUDED
#include "processor/procProvider.hpp"
#include "luaObjects.hpp"
#include "module/luaExtensionBuilder.hpp"
#include <vector>

namespace _Wolframe {
namespace langbind {

struct LuaScriptContext
{
	LuaModuleMap modulemap;
	LuaFunctionMap funcmap;

	LuaScriptContext() :funcmap(&modulemap){}
	~LuaScriptContext();

	void load( const std::vector<std::string>& prgfiles_, const module::ModulesDirectory* modules);

	void setDefaultFilter( const std::string& defaultfilter_)
	{
		m_defaultfilter = defaultfilter_;
	}

	///\brief Get the list of commands
	std::list<std::string> commands() const
	{
		return funcmap.commands();
	}

	const std::string& defaultfilter() const
	{
		return m_defaultfilter;
	}

private:
	std::vector<module::LuaExtensionConstructor*> m_objects;
	std::string m_defaultfilter;

private:
	LuaScriptContext( const LuaScriptContext&) :funcmap(&modulemap){}	//non copyable
};

}}//namespace
#endif
