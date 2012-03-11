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
#include "protocol/commandHandler.hpp"
#include "config/description.hpp"
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#if WITH_LUA
#include "langbind/appObjects.hpp"
#include "langbind/luaCommandConfig.hpp"
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
			( "name",	&ScriptConfigStruct::name)
			( "path",	&ScriptConfigStruct::path)
			( "main",	&ScriptConfigStruct::main)
			( "module",	&ScriptConfigStruct::module)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* FormConfigStruct::description()
{
	struct ThisDescription :public config::Description<FormConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",	&FormConfigStruct::name)
			( "path",	&FormConfigStruct::path)
			( "main",	&FormConfigStruct::main)
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
			( "input",	&DirectMapConfigStruct::input)
			( "output",	&DirectMapConfigStruct::output)
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
	:ConfigurationBase( "tproc", 0, "tproc") {}

#if WITH_LUA
static bool isLuaScript( const std::string& path)
{
	return (path.size()>4 && boost::algorithm::iequals( path.c_str()+path.size()-4, ".lua"));
}
#endif

static bool isSimpleForm( const std::string& path)
{
	return (path.size()>4 && boost::algorithm::iequals( path.c_str()+path.size()-4, ".frm"));
}

bool Configuration::defineScript( const ScriptConfigStruct& sc)
{
#if WITH_LUA
	if (isLuaScript( sc.path))
	{
		langbind::LuaCommandConfig* cfg;
		m_configs.push_back( cfg=new langbind::LuaCommandConfig( sc.main, sc.path, sc.module));
		protocol::CommandBase* cmd = new protocol::Command< langbind::LuaCommandHandler, langbind::LuaCommandConfig>( sc.name.c_str(), cfg);
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
	if (isSimpleForm( dm.input.path))
	{

	}
	else
	{
		LOG_ERROR << "Unknown type of DDL source loaded for input: " << dm.input.path;
		return false;
	}
	if (isSimpleForm( dm.output.path))
	{
	}
	else
	{
		LOG_ERROR << "Unknown type of DDL source loaded for output: " << dm.output.path;
		return false;
	}
	return true;
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
		if (!itr->get()->config()->test()) return false;
	}
	return true;
}

bool Configuration::check() const
{
	std::vector<CountedReference< protocol::CommandBase> >::const_iterator itr=m_cmds.begin(),end=m_cmds.end();
	for (;itr!=end; ++itr)
	{
		if (itr->get()->config())
		{
			if (!itr->get()->config()->check()) return false;
		}
	}
	return true;
}

void Configuration::print( std::ostream& o, size_t i) const
{
	std::vector<CountedReference< protocol::CommandBase> >::const_iterator itr=m_cmds.begin(),end=m_cmds.end();
	for (;itr!=end; ++itr)
	{
		if (itr->get()->config())
		{
			itr->get()->config()->print( o, i);
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
			boost::filesystem::path pt( itr->input.path);
			if (pt.is_absolute())
			{
				itr->input.path = pt.string();
			}
			else
			{
				itr->input.path = boost::filesystem::absolute( pt, boost::filesystem::path( refPath).branch_path()).string();
			}
		}
	}{
		std::vector<DirectMapConfigStruct>::iterator itr=m_data.directmap.begin(),end=m_data.directmap.end();
		for (;itr != end; ++itr)
		{
			boost::filesystem::path pt( itr->output.path);
			if (pt.is_absolute())
			{
				itr->output.path = pt.string();
			}
			else
			{
				itr->output.path = boost::filesystem::absolute( pt, boost::filesystem::path( refPath).branch_path()).string();
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


