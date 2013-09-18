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
///\file luaScriptContext.cpp
///\brief Implementation of the lua script objects configuration
#include "luaScriptContext.hpp"
#include "processor/moduleInterface.hpp"
#include "processor/moduleDirectory.hpp"
#include <algorithm>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

LuaScriptContext::~LuaScriptContext()
{
	std::vector<module::LuaExtensionConstructor*>::const_iterator ii=m_objects.begin(),ee=m_objects.end();
	for (; ii != ee; ++ii) delete *ii;
}

void LuaScriptContext::loadPrograms( const std::vector<std::string>& prgfiles_, const module::ModulesDirectory* modules)
{
	// load language extension module objects:
	module::ModulesDirectory::simpleBuilder_iterator mi = modules->objectsBegin(), me = modules->objectsEnd();
	for (int midx=0; mi != me; ++mi,++midx)
	{
		if (mi->objectType() == ObjectConstructorBase::LANGUAGE_EXTENSION_OBJECT)
		{
			const char* className = mi->objectClassName();
			if (module::LuaExtensionBuilder::classNameMatches( className))
			{
				//PF:HACK: Dangerous exchange of dynamic_cast by reinterpret_cast for the time being. It should at least work on all platforms
				module::LuaExtensionConstructor* co = reinterpret_cast< module::LuaExtensionConstructor* >((*mi)->constructor());
				if (!co) throw std::runtime_error( "Language extension module has Lua extension module identifier but is not of this class");
				modulemap.defineLuaModule( co->moduleName(), LuaModule( co->moduleName(), co->object()));
				m_objects.push_back( co);
			}
		}
	}

	// load scripts:
	std::vector<std::string>::const_iterator si=prgfiles_.begin(), se=prgfiles_.end();
	for (;si != se; ++si)
	{
		langbind::LuaScript script( *si);
		std::vector<std::string>::const_iterator ni = script.functions().begin(), ne = script.functions().end();
		for (; ni != ne; ++ni)
		{
			funcmap.defineLuaFunction( *ni, script);
		}
	}
}

std::vector<std::string> LuaScriptContext::loadProgram( const std::string& prgfile)
{
	std::vector<std::string> rt;
	langbind::LuaScript script( prgfile);
	std::vector<std::string>::const_iterator ni = script.functions().begin(), ne = script.functions().end();
	for (; ni != ne; ++ni)
	{
		rt.push_back( *ni);
		funcmap.defineLuaFunction( *ni, script);
	}
	return rt;
}



