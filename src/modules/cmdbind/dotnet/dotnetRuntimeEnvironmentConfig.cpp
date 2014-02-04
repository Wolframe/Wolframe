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
///\file dotnetRuntimeEnvironmentConfig.cpp
///\brief Implementation .NET runtime environment configuration
#include "dotnetRuntimeEnvironmentConfig.hpp"
#include "logger-v1.hpp"
#include "utils/fileUtils.hpp"
#include <cstring>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::module;

DotnetRuntimeEnvironmentConfig::AssemblyDescription::AssemblyDescription( const std::string& description_)
	:description(description_)
{
	const char* ee = std::strchr( description.c_str(), ',');
	if (ee == 0) ee = description.c_str() + description.size();
	name.append( boost::algorithm::trim_copy( std::string( description.c_str(), ee - description.c_str())));
}

bool DotnetRuntimeEnvironmentConfig::parse( const config::ConfigurationTree& pt, const std::string&, const ModulesDirectory*)
{
	boost::property_tree::ptree::const_iterator pi = pt.begin(), pe = pt.end();
	for (; pi != pe; ++pi)
	{
		if (boost::iequals( pi->first, "clrversion"))
		{
			m_clrversion = pi->second.get_value<std::string>();
		}
		else if (boost::iequals( pi->first, "typelibpath"))
		{
			m_typelibpath = pi->second.get_value<std::string>();
		}
		else if (boost::iequals( pi->first, "assembly"))
		{
			m_assemblylist.push_back( pi->second.get_value<std::string>());
		}
		else
		{
			LOG_ERROR << "expected 'program' or 'filter' definition instead of '" << pi->first << "'";
			return false;
		}
	}
	return true;
}

void DotnetRuntimeEnvironmentConfig::setCanonicalPathes( const std::string& referencePath)
{
	m_typelibpath = utils::getCanonicalPath( m_typelibpath, referencePath);
}

bool DotnetRuntimeEnvironmentConfig::check() const
{
	bool rt = true;
	if (!utils::fileExists( m_typelibpath))
	{
		LOG_ERROR << ".NET type library path '" << m_typelibpath << "' does not exist";
		rt = false;
	}
	return rt;
}

void DotnetRuntimeEnvironmentConfig::print( std::ostream& os, size_t indent) const
{
	std::string indentstr( indent, ' ');
	os << indentstr << "Configuration of '" << m_className << "'" << std::endl;
	os << indentstr << "Version CLR: " << m_clrversion << std::endl;
	os << indentstr << "Path of Type Library Descriptions: " << m_typelibpath << std::endl;
	
	std::vector<AssemblyDescription>::const_iterator ai = m_assemblylist.begin(), ae = m_assemblylist.end();
	for (; ai != ae; ++ai)
	{
		os << indentstr << "Assembly Declaration of " << ai->name << ": '" << ai->description << "'" << std::endl;
	}
}


