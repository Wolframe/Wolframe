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
///\file langbind_appObjects.cpp
///\brief implementation of scripting global context
#include "langbind/appGlobalContext.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace langbind;

static GlobalContextR g_context;

void _Wolframe::langbind::defineGlobalContext( const GlobalContextR& context)
{
	g_context = context;
}

GlobalContext* _Wolframe::langbind::getGlobalContext()
{
	return g_context.get();
}

struct AutoCreateGlobalContext
{
	AutoCreateGlobalContext()
	{
		g_context.reset( new GlobalContext());
	}
};
AutoCreateGlobalContext g_autoCreateGlobalContext;

bool GlobalContext::load( const ApplicationEnvironmentConfig& config)
{
	bool rt = true;
	{
		std::vector<DDLCompilerConfigStruct>::const_iterator itr=config.data().DDL.begin(),end=config.data().DDL.end();
		for (;itr!=end; ++itr)
		{
			LOG_INFO << "Loading DDL compiler " << itr->name;
			///TODO: load compiler from module itr->modulepath
			///Call defineDDLCompiler( const char* name, const ddl::CompilerInterfaceR& ci);
		}
	}
	{
		std::vector<DDLFormConfigStruct>::const_iterator itr=config.data().form.begin(),end=config.data().form.end();
		for (;itr!=end; ++itr)
		{
			LOG_INFO << "Loading DDL form " << itr->name;
			ddl::CompilerInterfaceR ci;
			if (!getDDLCompiler( itr->DDL.c_str(), ci))
			{
				LOG_ERROR << "Unknown DDL of form" << itr->DDL;
				rt = false;
			}
			else
			{
				ddl::StructType form;
				std::string error;
				if (!ci->compileFile( itr->sourcepath, form, error))
				{
					LOG_ERROR << "Error in DDL form source: " << error;
					rt = false;
				}
				else
				{
					defineForm( itr->name.c_str(), form);
				}
			}
		}
	}
	{
		std::vector<FilterConfigStruct>::const_iterator itr=config.data().filter.begin(),end=config.data().filter.end();
		for (;itr!=end; ++itr)
		{
			LOG_INFO << "Loading filter " << itr->name;
			///TODO: load filter from itr->modulepath
			///Call defineFilter( const char* name, const FilterFactoryR& f);
		}
	}
	{
		std::vector<TransactionFunctionConfigStruct>::const_iterator itr=config.data().transaction.begin(),end=config.data().transaction.end();
		for (;itr!=end; ++itr)
		{
			LOG_INFO << "Loading transaction function " << itr->name;
			///TODO: load transaction function from itr->modulepath
			///Call defineTransactionFunction( const char* name, const TransactionFunction& f);
		}
	}
#if WITH_LUA
	{
		std::vector<ScriptCommandConfigStruct>::const_iterator itr=config.data().script.begin(),end=config.data().script.end();
		for (;itr!=end; ++itr)
		{
			defineLuaFunction( itr->name.c_str(), itr->sourcepath.c_str());
		}
	}
#endif
	{
		std::vector<PluginModuleAPIConfigStruct>::const_iterator itr=config.data().plugin.begin(),end=config.data().plugin.end();
		for (;itr!=end; ++itr)
		{
			LOG_INFO << "Loading plugin module function " << itr->name;
			///TODO: load plugin module function from itr->modulepath
			///API for defining the function is not yet available !
		}
	}
	return rt;
}


