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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file WolfwizardCommandLine.cpp
///\brief Implementation of the options of a wolfwizard call
#include "wolfwizardCommandLine.hpp"
#include "utils/fileUtils.hpp"
#include "logger-v1.hpp"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <cstring>
#include <algorithm>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::config;

namespace po = boost::program_options;

struct WolfwizardOptionStruct
{
	po::options_description fopt;
	po::positional_options_description popt;

	WolfwizardOptionStruct()
		:fopt("Options")
	{
		fopt.add_options()
			( "version,v", "print version" )
			( "help,h", "print help message" )
			( "config", po::value<std::string>(), "specify configuration file to load" )
			;
		popt.add( "cmd", 1);
	}
};

config::ConfigurationNode WolfwizardCommandLine::getConfigNode( const std::string& name) const
{
	return m_config.root().getChildrenJoined( name);
}

std::list<std::string> WolfwizardCommandLine::configModules() const
{
	std::list<std::string> rt;
	config::ConfigurationNode module_section = getConfigNode( "LoadModules");
	std::string directory;

	types::PropertyTree::Node::const_iterator mi = module_section.begin(), me = module_section.end();
	for (; mi != me; ++mi)
	{
		if (boost::algorithm::iequals( mi->first, "directory"))
		{
			if (!directory.empty()) throw std::runtime_error( "duplicate definition of 'directory' in section LoadModules");
			directory = mi->second.data();
			if (directory.empty()) throw std::runtime_error( "empty definition of 'directory' in section LoadModules");
		}
	}
	mi = module_section.begin();
	for (; mi != me; ++mi)
	{
		if (boost::algorithm::iequals( mi->first, "module"))
		{
			std::string absmodpath = m_modulesDirectory->getAbsoluteModulePath( mi->second.data(), directory);
			if (absmodpath.empty())
			{
				throw std::runtime_error( std::string("could not resolve configured module path '") + mi->second.data() + "'");
			}
			rt.push_back( absmodpath);
		}
	}
	return rt;
}

WolfwizardCommandLine::WolfwizardCommandLine( int argc, char** argv, const std::string& referencePath_)
	:m_printhelp(false)
	,m_printversion(false)
	,m_modulesDirectory(0)
	,m_configurationPath(referencePath_)
{
	static const WolfwizardOptionStruct ost;
	po::variables_map vmap;
	try
	{
		po::store( po::command_line_parser(argc, argv).options(ost.fopt).run(), vmap);
		po::notify( vmap);
	}
	catch (std::exception& e)
	{
		throw std::runtime_error( std::string("error parsing command line options: ") + e.what());
	}
	m_printversion = vmap.count( "version");
	m_printhelp = vmap.count( "help");
	if (vmap.count( "config"))
	{
		m_configfile = vmap["config"].as<std::string>();
		m_configfile = utils::getCanonicalPath( m_configfile, m_configurationPath);
		m_configurationPath = boost::filesystem::path( m_configfile ).branch_path().string();
		m_config = utils::readPropertyTreeFile( m_configfile);
		m_modulesDirectory = new module::ModulesDirectory( m_configurationPath);
		m_modules = configModules();

		if (!m_modulesDirectory->loadModules( m_modules))
		{
			throw std::runtime_error( "Modules could not be loaded");
		}
	}
}

void WolfwizardCommandLine::print( std::ostream& out)
{
	static const WolfwizardOptionStruct ost;
	out << "Call:" << std::endl;
	out << "\twolfwizard [OPTION]" << std::endl;
	out << ost.fopt << std::endl;
}



