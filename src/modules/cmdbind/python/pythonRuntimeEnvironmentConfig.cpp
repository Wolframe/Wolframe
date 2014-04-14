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
//\file pythonRuntimeEnvironmentConfig.cpp
//\brief Implementation of python runtime environment configuration
#include "pythonRuntimeEnvironmentConfig.hpp"
#include "types/string.hpp"
#include "utils/fileUtils.hpp"
#include <Python.h>

using namespace _Wolframe;
using namespace _Wolframe::module;

bool PythonRuntimeEnvironmentConfig::parse( const config::ConfigurationNode& pt, const std::string&, const ModulesDirectory* modules)
{
	unsigned int pathcnt = 0;
	config::ConfigurationNode::const_iterator pi = pt.begin(), pe = pt.end();
	for (; pi != pe; ++pi)
	{
		if (boost::iequals( pi->first, "script"))
		{
			m_scripts.push_back( pi->second.data());
		}
		else if (boost::iequals( pi->first, "path"))
		{
			m_path = pi->second.data();
			if (++pathcnt > 1) throw std::runtime_error("specified path two times in python config");
		}
	}
}

void PythonRuntimeEnvironmentConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indentstr( indent, ' ');

	os << indentstr << "Configuration of '" << m_className << "'" << std::endl;
	std::vector<std::string>::const_iterator si = m_scripts.begin(), se = m_scripts.end();
	for (; si != se; ++si)
	{
		os << indentstr << "Script '" << *si << "'" << std::endl;
	}
}

void PythonRuntimeEnvironmentConfig::global_startup( const char* path)
{
	types::String pathstr( path);
	std::wstring pathwstr( pathstr.towstring());

	Py_SetProgramName( const_cast<wchar_t*>(pathwstr.c_str()));
	// initialize Python without signal handlers (initsigs=0)
	Py_InitializeEx(0/*initsigs*/);
	// initialize thread support
	PyEval_InitThreads();
	//[+] Py_SetStandardStreamEncoding( "UTF-8", 0);
}

void PythonRuntimeEnvironmentConfig::global_shutdown()
{
	// shut down the interpreter
	PyEval_AcquireLock();
	Py_Finalize();
}

void PythonRuntimeEnvironmentConfig::setCanonicalPathes( const std::string& referencePath)
{
	if (m_path.empty())
	{
		m_path = referencePath;
	}
	else
	{
		m_path = utils::getCanonicalPath( m_path, referencePath);
	}
	std::vector<std::string>::iterator si = m_scripts.begin(), se = m_scripts.end();
	for (; si != se; ++si)
	{
		*si = utils::getCanonicalPath( *si, m_path);
	}
	if (!m_initialized)
	{
		global_startup( m_path.c_str());
		m_initialized = true;
	}
}


PythonRuntimeEnvironmentConfig::~PythonRuntimeEnvironmentConfig()
{
	if (m_initialized)
	{
		global_shutdown();
		m_initialized = false;
	}
}

