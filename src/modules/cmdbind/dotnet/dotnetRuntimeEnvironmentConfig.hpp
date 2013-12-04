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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
//\file dotnetRuntimeEnvironmentConfig.hpp
//\brief Interface .NET runtime environment configuration
#ifndef _Wolframe_DOTNET_RUNTIME_ENVIRONMENT_CONFIGURATION_HPP_INCLUDED
#define _Wolframe_DOTNET_RUNTIME_ENVIRONMENT_CONFIGURATION_HPP_INCLUDED
#include "prgbind/runtimeEnvironmentConstructor.hpp"
#include "config/ConfigurationTree.hpp"
#include "processor/moduleInterface.hpp"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <map>

namespace _Wolframe {
namespace module {

///\brief Named configuration definition
class DotnetRuntimeEnvironmentConfig
	:public config::NamedConfiguration
{
public:
	class AssemblyDescription
	{
	public:
		std::string name;
		std::string description;

		AssemblyDescription(){}
		AssemblyDescription( const std::string& description_);
		AssemblyDescription( const AssemblyDescription& o)
			:name(o.name),description(o.description){}
	};

public:
	DotnetRuntimeEnvironmentConfig( const char* className_, const char* name, const char* logParent, const char* logName)
		:config::NamedConfiguration( name, logParent, logName)
		,m_className(className_){}
	virtual ~DotnetRuntimeEnvironmentConfig(){}

	///\brief Parse the configuration
	///\param[in] pt configuration tree
	///\param[in] modules module directory
	virtual bool parse( const config::ConfigurationTree& pt, const std::string&, const ModulesDirectory* modules);

	///\brief Set canonical path for files referenced as relative path in configuration
	///\param[in] referencePath reference path
	virtual void setCanonicalPathes( const std::string& referencePath);

	virtual bool check() const;

	virtual bool checkReferences( const proc::ProcessorProvider*) const {return true;}

	virtual void print( std::ostream& os, size_t indent ) const;

	virtual const char* className() const
	{
		return m_className;
	}

	const std::vector<AssemblyDescription>& assemblylist() const
	{
		return m_assemblylist;
	}

	const std::string& clrversion() const
	{
		return m_clrversion;
	}

	const std::string& typelibpath() const
	{
		return m_typelibpath;
	}

private:
	std::string m_clrversion;
	std::string m_typelibpath;

	std::vector<AssemblyDescription> m_assemblylist;
	const char* m_className;
};

}} //namespace
#endif

