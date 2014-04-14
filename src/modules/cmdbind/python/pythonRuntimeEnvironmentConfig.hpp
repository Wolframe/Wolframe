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
///\file pythonRuntimeEnvironmentConfig.hpp
///\brief Interface for python runtime environment configuration
#ifndef _Wolframe_Python_RUNTIME_ENVIRONMENT_CONFIGURATION_HPP_INCLUDED
#define _Wolframe_Python_RUNTIME_ENVIRONMENT_CONFIGURATION_HPP_INCLUDED
#include "config/configurationTree.hpp"
#include "module/moduleInterface.hpp"
#include "processor/procProviderInterface.hpp"
#include <string>
#include <vector>
#include <map>

namespace _Wolframe {
namespace module {

///\brief Named configuration definition
class PythonRuntimeEnvironmentConfig
	:public config::NamedConfiguration
{
public:
	PythonRuntimeEnvironmentConfig( const char* className_, const char* name, const char* logParent, const char* logName)
		:config::NamedConfiguration( name, logParent, logName)
		,m_className(className_)
		,m_initialized(false){}
	virtual ~PythonRuntimeEnvironmentConfig();

	///\brief Parse the configuration
	///\param[in] pt configuration tree
	///\param[in] modules module directory
	virtual bool parse( const config::ConfigurationNode& pt, const std::string&, const ModulesDirectory* modules);

	///\brief Set canonical path for files referenced as relative path in configuration
	///\param[in] referencePath reference path
	virtual void setCanonicalPathes( const std::string& referencePath);

	virtual bool check() const							{return true;}

	virtual bool checkReferences( const proc::ProcessorProviderInterface*) const	{return true;}

	virtual void print( std::ostream& os, size_t indent ) const;

	virtual const char* className() const						{return m_className;}

private:
	static void global_startup( const char* path);
	static void global_shutdown();

private:
	const char* m_className;
	std::vector<std::string> m_scripts;
	std::string m_path;
	bool m_initialized;
};

}} //namespace
#endif

