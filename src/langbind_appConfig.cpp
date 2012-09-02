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
#include "langbind/appGlobalContext.hpp"
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
	GlobalContext lgct(0);
	return lgct.load( m_config);
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

	m_config.setCanonicalPathes( referencePath);
}

