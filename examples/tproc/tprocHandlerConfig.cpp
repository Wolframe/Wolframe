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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file tprocHandlerConfig.cpp
///\brief Implementation of the commands of the tproc connection handler
#include "countedReference.hpp"
#include "tprocHandlerConfig.hpp"
#include "langbind/ioFilterCommandHandler.hpp"
#include "langbind/directmapCommandEnvironment.hpp"
#include "langbind/directmapCommandHandler.hpp"
#include "ddl/compiler/simpleFormCompiler.hpp"
#include "protocol/commandHandler.hpp"
#include "config/description.hpp"
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

#if WITH_LUA
#include "langbind/appObjects.hpp"
#include "langbind/luaCommandEnvironment.hpp"
#include "langbind/luaCommandHandler.hpp"
#endif

using namespace _Wolframe;
using namespace _Wolframe::tproc;

const config::DescriptionBase* ScriptConfigStruct::description()
{
	struct ThisDescription :public config::Description<ScriptConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "cmd",	&ScriptConfigStruct::cmdname)
			( "path",	&ScriptConfigStruct::path)
			( "main",	&ScriptConfigStruct::main)
			( "module",	&ScriptConfigStruct::module)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* DirectMapConfigStruct::description()
{
	struct ThisDescription :public config::Description<DirectMapConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "cmd",	&DirectMapConfigStruct::cmdname)
			( "ddl",	&DirectMapConfigStruct::ddlname)
			( "filter",	&DirectMapConfigStruct::filtername)
			( "inputform",	&DirectMapConfigStruct::inputform)
			( "outputform",	&DirectMapConfigStruct::outputform)
			( "function",	&DirectMapConfigStruct::function)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* ConfigurationStruct::description()
{
	struct ThisDescription :public config::Description<ConfigurationStruct>
	{
		ThisDescription()
		{
			(*this)
			( "directmap",	&ConfigurationStruct::directmap)
			( "script",	&ConfigurationStruct::script)
			( "inputbuf",	&ConfigurationStruct::input_bufsize)
			( "outputbuf",	&ConfigurationStruct::output_bufsize)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}


Configuration::Configuration()
	:ConfigurationBase( "tproc", 0, "tproc")
{
	m_compilers.push_back( new ddl::SimpleFormCompiler());
}

#if WITH_LUA
static bool isLuaScript( const std::string& path)
{
	return (path.size()>4 && boost::algorithm::iequals( path.c_str()+path.size()-4, ".lua"));
}
#endif

bool Configuration::defineScript( const ScriptConfigStruct& sc)
{
#if WITH_LUA
	if (isLuaScript( sc.path))
	{
		langbind::LuaCommandEnvironment* env;
		m_envs.push_back( env=new langbind::LuaCommandEnvironment( sc.main, sc.path, sc.module));
		protocol::CommandBase* cmd = new protocol::Command< langbind::LuaCommandHandler, langbind::LuaCommandEnvironment>( sc.cmdname.c_str(), env);
		m_cmds.push_back( cmd);
	}
	else
#endif
	{
		LOG_ERROR << "Unknown type of script loaded: " << sc.path;
		return false;
	}
	return true;
}

bool Configuration::defineDirectMap( const DirectMapConfigStruct& dm)
{
	std::vector<CountedReference<ddl::CompilerInterface> >::const_iterator itr=m_compilers.begin(),end=m_compilers.end();
	for (; itr != end; ++itr)
	{
		ddl::CompilerInterface* ddlc = itr->get();
		if (ddlc && boost::algorithm::iequals( dm.ddlname, ddlc->ddlname()))
		{
			langbind::DirectmapCommandEnvironment* env;
			m_envs.push_back( env=new langbind::DirectmapCommandEnvironment( ddlc, dm.filtername, dm.inputform, dm.outputform, dm.function));
			protocol::CommandBase* cmd = new protocol::Command< langbind::DirectmapCommandHandler, langbind::DirectmapCommandEnvironment>( dm.cmdname.c_str(), env);
			m_cmds.push_back( cmd);
			return true;
		}
	}
	LOG_ERROR << "Unknown type of DDL: '" << dm.ddlname << "'";
	return false;
}

bool Configuration::parse( const config::ConfigurationTree& pt, const std::string&, const module::ModulesDirectory*)
{
	try
	{
		std::string errmsg;
		if (!m_data.description()->parse( (void*)&m_data, (const boost::property_tree::ptree&)pt, errmsg))
		{
			LOG_ERROR << "Error in configuration: " << errmsg;
			return false;
		}
		return true;
	}
	catch (std::exception& e)
	{
		LOG_ERROR << "Error parsing configuration: " << e.what();
		return false;
	}
}

bool Configuration::test() const
{
	std::vector< CountedReference<protocol::CommandBase> >::const_iterator itr=m_cmds.begin(),end=m_cmds.end();
	for (;itr!=end; ++itr)
	{
		if (!itr->get()->env()->test()) return false;
	}
	return true;
}

bool Configuration::check() const
{
	std::vector<CountedReference< protocol::CommandBase> >::const_iterator itr=m_cmds.begin(),end=m_cmds.end();
	for (;itr!=end; ++itr)
	{
		if (itr->get()->env())
		{
			if (!itr->get()->env()->check()) return false;
		}
	}
	return true;
}

void Configuration::print( std::ostream& o, size_t i) const
{
	std::vector<CountedReference< protocol::CommandBase> >::const_iterator itr=m_cmds.begin(),end=m_cmds.end();
	for (;itr!=end; ++itr)
	{
		if (itr->get()->env())
		{
			itr->get()->env()->print( o, i);
		}
	}
}

void Configuration::setCanonicalPathes( const std::string& refPath)
{
	{
		std::vector<ScriptConfigStruct>::iterator itr=m_data.script.begin(),end=m_data.script.end();
		for (;itr != end; ++itr)
		{
			boost::filesystem::path pt(itr->path);
			if (pt.is_absolute())
			{
				itr->path = pt.string();
			}
			else
			{
				itr->path = boost::filesystem::absolute( pt, boost::filesystem::path( refPath).branch_path()).string();
			}
		}
	}{
		std::vector<DirectMapConfigStruct>::iterator itr=m_data.directmap.begin(),end=m_data.directmap.end();
		for (;itr != end; ++itr)
		{
			boost::filesystem::path pt( itr->inputform);
			if (pt.is_absolute())
			{
				itr->inputform = pt.string();
			}
			else
			{
				itr->inputform = boost::filesystem::absolute( pt, boost::filesystem::path( refPath).branch_path()).string();
			}
		}
	}{
		std::vector<DirectMapConfigStruct>::iterator itr=m_data.directmap.begin(),end=m_data.directmap.end();
		for (;itr != end; ++itr)
		{
			boost::filesystem::path pt( itr->outputform);
			if (pt.is_absolute())
			{
				itr->outputform = pt.string();
			}
			else
			{
				itr->outputform = boost::filesystem::absolute( pt, boost::filesystem::path( refPath).branch_path()).string();
			}
		}
	}{
		std::vector<ScriptConfigStruct>::iterator itr=m_data.script.begin(),end=m_data.script.end();
		for (;itr != end; ++itr)
		{
			if (!defineScript( *itr)) throw std::runtime_error( "direct map load error");
		}
	}{
		std::vector<DirectMapConfigStruct>::iterator itr=m_data.directmap.begin(),end=m_data.directmap.end();
		for (;itr != end; ++itr)
		{
			if (!defineDirectMap( *itr)) throw std::runtime_error( "direct map load error");
		}
	}
}

const std::vector<CountedReference< protocol::CommandBase> >& Configuration::getCommands( const char* ) const
{
	return m_cmds;
}


