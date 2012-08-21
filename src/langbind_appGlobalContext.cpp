/************************************************************************
Copyright (C) 2011, 2012 Project Wolframe.
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
#include "utils/miscUtils.hpp"
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
			///Call defineDDLCompiler( const std::string& name, const ddl::CompilerInterfaceR& ci);
		}
	}
	{
		std::vector<DDLFormConfigStruct>::const_iterator itr=config.data().form.begin(),end=config.data().form.end();
		for (;itr!=end; ++itr)
		{
			LOG_INFO << "Loading DDL form " << itr->sourcepath;
			ddl::CompilerInterfaceR ci;
			if (!getDDLCompiler( itr->DDL, ci))
			{
				LOG_ERROR << "Unknown DDL of form" << itr->DDL;
				rt = false;
			}
			else
			{
				ddl::StructTypeR form = ddl::StructTypeR( new ddl::StructType());
				std::string error;
				*form = ci->compileFile( itr->sourcepath);
				if (form->doctype())
				{
					std::string name = ddl::StructType::getIdFromDoctype( form->doctype());
					defineForm( name, form);
				}
				else
				{
					std::string name = utils::getFileStem( itr->sourcepath);
					defineForm( name, form);
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
			///Call defineFilter( const std::string& name, const FilterFactoryR& f);
		}
	}
	{
		std::vector<PeerFunctionConfigStruct>::const_iterator itr=config.data().peerfunction.begin(),end=config.data().peerfunction.end();
		for (;itr!=end; ++itr)
		{
			LOG_INFO << "Loading peer function " << itr->name;
			///TODO: load peer function from itr->modulepath
			///Call definePeerFunction( const std::string& name, const PeerFunction& f);
		}
	}
	{
		std::vector<PeerFormFunctionConfigStruct>::const_iterator itr=config.data().peerformfunction.begin(),end=config.data().peerformfunction.end();
		for (;itr!=end; ++itr)
		{
			LOG_INFO << "Loading peer form function function " << itr->name;
			///TODO: load peer form function from itr->modulepath
			///Call definePeerFormFunction( const std::string& name, const PeerFormFunction& f);
		}
	}
#if WITH_LUA
	{
		std::vector<ScriptCommandConfigStruct>::const_iterator itr=config.data().script.begin(),end=config.data().script.end();
		for (;itr!=end; ++itr)
		{
			defineLuaFunction( itr->name, itr->sourcepath);
		}
	}
#endif
	{
		std::vector<FormFunctionConfigStruct>::const_iterator itr=config.data().formfunction.begin(),end=config.data().formfunction.end();
		for (;itr!=end; ++itr)
		{
			LOG_INFO << "Loading form function" << itr->name;
			///TODO: load form function from itr->modulepath
			///API for defining the function is not yet available !
		}
	}
	return rt;
}



