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
///\file luaCommandHandlerConfig.cpp
///\brief Implementation of the lua command handler configuration
#include "luaCommandHandlerConfig.hpp"
#include "logger-v1.hpp"
#include "utils/fileUtils.hpp"
#include "utils/stringUtils.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;

bool LuaCommandHandlerConfig::parse( const config::ConfigurationNode& pt, const std::string&, const module::ModulesDirectory* modules)
{
	m_modules = modules;
	config::ConfigurationNode::const_iterator pi = pt.begin(), pe = pt.end();
	try
	{
		for (; pi != pe; ++pi)
		{
			if (boost::iequals( pi->first, "filter"))
			{
				std::vector<std::string> filterdef;
				utils::splitString( filterdef, pi->second.data().string(), "=");
				if (filterdef.size() == 1)
				{
					m_filtermap.insert( "", filterdef.at(0));
				}
				else if (filterdef.size() == 2)
				{
					m_filtermap.insert( filterdef.at(0), filterdef.at(1));
				}
				else
				{
					throw std::runtime_error( "illegal value for filter declaration. expected two items separated by a '='");
				}
			}
			else if (boost::iequals( pi->first, "program"))
			{
				m_programfiles.push_back( pi->second.data());
			}
			else
			{
				throw std::runtime_error( std::string("expected 'program' or 'filter' definition instead of '") + pi->first + "'");
			}
		}
	}
	catch (std::runtime_error& e)
	{
		LOG_ERROR << "lua command handler configuration error " << pi->second.data().position.logtext() << ":" << e.what();
		return false;
	}
	return true;
}

void LuaCommandHandlerConfig::setCanonicalPathes( const std::string& referencePath)
{
	std::vector<std::string>::iterator pi = m_programfiles.begin(), pe = m_programfiles.end();
	for (; pi != pe; ++pi)
	{
		*pi = utils::getCanonicalPath( *pi, referencePath);
	}
}

bool LuaCommandHandlerConfig::check() const
{
	bool rt = true;
	std::vector<std::string>::const_iterator pi = m_programfiles.begin(), pe = m_programfiles.end();
	for (; pi != pe; ++pi)
	{
		if (!utils::fileExists( *pi))
		{
			LOG_ERROR << "program file '" << *pi << "' does not exist";
			rt = false;
		}
	}
	return rt;
}

void LuaCommandHandlerConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indentstr( indent*3, ' ');
	std::vector<std::string>::const_iterator pi = m_programfiles.begin(), pe = m_programfiles.end();
	for (; pi != pe; ++pi)
	{
		os << indentstr << "program " << *pi;
	}
}

