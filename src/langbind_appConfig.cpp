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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file langbind_appConfig.cpp
///\brief Implementation of the language binding objects configuration
#include "langbind/appConfig.hpp"
#include "config/structSerialize.hpp"
#include "logger-v1.hpp"
#include <string>
#include <vector>
#include <map>
#include "utils/miscUtils.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace langbind;

bool ApplicationEnvironmentConfig::check() const
{
	bool rt = true;
	std::map<std::string,bool> filters;
	std::map<std::string,bool> compilers;
	std::map<std::string,bool> functions;
	std::map<std::string,bool> scripts;
	{
		compilers[ "simpleform"] = true;
		std::vector<DDLCompilerConfigStruct>::const_iterator itr=m_config.DDL.begin(),end=m_config.DDL.end();
		for (;itr!=end; ++itr)
		{
			if (compilers[ itr->name])
			{
				LOG_ERROR << "Duplicate definition or using reserved name for compiler " << itr->name;
				rt = false;
			}
			compilers[ itr->name] = true;
			if (!utils::fileExists( itr->modulepath))
			{
				LOG_ERROR << "Path of DDL compiler module does not exist: " << itr->modulepath;
				rt = false;
			}
		}
	}
	{
		std::vector<DDLFormConfigStruct>::const_iterator itr=m_config.form.begin(),end=m_config.form.end();
		for (;itr!=end; ++itr)
		{
			if (!compilers[ itr->DDL])
			{
				LOG_ERROR << "Undefined DDL used for form definition" << itr->sourcepath;
				rt = false;
			}
			if (!utils::fileExists( itr->sourcepath))
			{
				LOG_ERROR << "Path of DDL form source file does not exist: " << itr->sourcepath;
				rt = false;
			}
		}
	}
	{
		filters[ "char"] = true;
		filters[ "line"] = true;
		filters[ "xml:textwolf"] = true;
		filters[ "xml:libxml2"] = true;
		filters[ "xml:xmllite"] = true;
		filters[ "xml:msxml"] = true;
		std::vector<FilterConfigStruct>::const_iterator itr=m_config.filter.begin(),end=m_config.filter.end();
		for (;itr!=end; ++itr)
		{
			if (filters[ itr->name])
			{
				LOG_ERROR << "Duplicate definition or using reserved name for filter " << itr->name;
				rt = false;
			}
			filters[ itr->name] = true;
			if (!utils::fileExists( itr->modulepath))
			{
				LOG_ERROR << "Path of filter module does not exist: " << itr->modulepath;
				rt = false;
			}
		}
	}
	{
		std::vector<PeerFunctionConfigStruct>::const_iterator itr=m_config.peerfunction.begin(),end=m_config.peerfunction.end();
		for (;itr!=end; ++itr)
		{
			if (functions[ itr->name])
			{
				LOG_ERROR << "Duplicate definition or using reserved name for peer function " << itr->name;
				rt = false;
			}
			functions[ itr->name] = true;
			if (!filters[ itr->filter])
			{
				LOG_ERROR << "Undefined filter '" << itr->filter << "' used for peer function command reader/writer in " << itr->name;
				rt = false;
			}
			if (!utils::fileExists( itr->modulepath))
			{
				LOG_ERROR << "Path of peer function command handler module does not exist: " << itr->modulepath;
				rt = false;
			}
		}
	}
	{
		std::vector<ScriptCommandConfigStruct>::const_iterator itr=m_config.script.begin(),end=m_config.script.end();
		for (;itr!=end; ++itr)
		{
			if (scripts[ itr->name])
			{
				LOG_ERROR << "Duplicate definition of script procedure: " << itr->name;
				rt = false;
			}
			scripts[ itr->name] = true;
			if (!utils::fileExists( itr->sourcepath))
			{
				LOG_ERROR << "Path of script procedure does not exist: " << itr->sourcepath;
				rt = false;
			}
		}
	}
	{
		std::vector<FormFunctionConfigStruct>::const_iterator itr=m_config.formfunction.begin(),end=m_config.formfunction.end();
		for (;itr!=end; ++itr)
		{
			if (functions[ itr->name])
			{
				LOG_ERROR << "Duplicate definition or using reserved name for form function " << itr->name;
				rt = false;
			}
			functions[ itr->name] = true;
			if (!utils::fileExists( itr->modulepath))
			{
				LOG_ERROR << "Path of form function module does not exist: " << itr->modulepath;
				rt = false;
			}
		}
	}
	{
		std::vector<PeerFormFunctionConfigStruct>::const_iterator itr=m_config.peerformfunction.begin(),end=m_config.peerformfunction.end();
		for (;itr!=end; ++itr)
		{
			if (functions[ itr->name])
			{
				LOG_ERROR << "Duplicate definition or using reserved name for peer form function " << itr->name;
				rt = false;
			}
			functions[ itr->name] = true;
			if (!functions[ itr->peerfunc])
			{
				LOG_ERROR << "Undefined peer function '" << itr->name << "'";
				rt = false;
			}
		}
	}
	return rt;
}

bool ApplicationEnvironmentConfig::parse( const config::ConfigurationTree& pt, const std::string&, const module::ModulesDirectory*)
{
	try
	{
		config::parseConfigStructure( m_config, (const boost::property_tree::ptree&)pt);
		return true;
	}
	catch (std::exception& e)
	{
		LOG_ERROR << "Error parsing configuration: " << e.what();
		return false;
	}
}

void ApplicationEnvironmentConfig::print( std::ostream& os, std::size_t indent) const
{
	while (indent--) os << "\t";
	os << config::structureToString( m_config);
}

void ApplicationEnvironmentConfig::setCanonicalPathes( const std::string& referencePath)
{
	{
		std::vector<DDLCompilerConfigStruct>::iterator itr=m_config.DDL.begin(),end=m_config.DDL.end();
		for (;itr!=end; ++itr)
		{
			itr->modulepath = utils::getCanonicalPath( itr->modulepath, referencePath);
		}
	}
	{
		std::vector<DDLFormConfigStruct>::iterator itr=m_config.form.begin(),end=m_config.form.end();
		for (;itr!=end; ++itr)
		{
			itr->sourcepath = utils::getCanonicalPath( itr->sourcepath, referencePath);
		}
	}
	{
		std::vector<FilterConfigStruct>::iterator itr=m_config.filter.begin(),end=m_config.filter.end();
		for (;itr!=end; ++itr)
		{
			itr->modulepath = utils::getCanonicalPath( itr->modulepath, referencePath);
		}
	}
	{
		std::vector<PeerFunctionConfigStruct>::iterator itr=m_config.peerfunction.begin(),end=m_config.peerfunction.end();
		for (;itr!=end; ++itr)
		{
			itr->modulepath = utils::getCanonicalPath( itr->modulepath, referencePath);
		}
	}
	{
		std::vector<ScriptCommandConfigStruct>::iterator itr=m_config.script.begin(),end=m_config.script.end();
		for (;itr!=end; ++itr)
		{
			itr->sourcepath = utils::getCanonicalPath( itr->sourcepath, referencePath);
		}
	}
	{
		std::vector<FormFunctionConfigStruct>::iterator itr=m_config.formfunction.begin(),end=m_config.formfunction.end();
		for (;itr!=end; ++itr)
		{
			itr->modulepath = utils::getCanonicalPath( itr->modulepath, referencePath);
		}
	}
}

