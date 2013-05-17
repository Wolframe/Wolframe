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
#include "utils/miscUtils.hpp"
#include "logger-v1.hpp"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <cstring>
#include <algorithm>
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
			( "config,c", po::value<std::string>(), "specify configuration file to load" )
			;
	}
};


WolfwizardCommandLine::WolfwizardCommandLine( int argc, char** argv, const std::string& referencePath_, const std::string& modulePath)
	:m_printhelp(false)
	,m_printversion(false)
	,m_referencePath(referencePath_)
	,m_modulePath(modulePath)
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
		if (m_configfile.size() == 0 || m_configfile[0] != '.')
		{
			m_configfile = utils::getCanonicalPath( m_configfile, m_referencePath);
		} else {
			m_referencePath = boost::filesystem::path( m_configfile ).branch_path().string();
		}
		boost::property_tree::ptree ptcfg = utils::readPropertyTreeFile( m_configfile);

		boost::property_tree::ptree::const_iterator gi = ptcfg.begin(), ge = ptcfg.end();
		for (; gi != ge; ++gi)
		{
			if (boost::algorithm::iequals( gi->first, "Processor"))
			{
				m_providerconfig = gi->second;
				break;
			}
			if (boost::algorithm::iequals( gi->first, "LoadModules"))
			{
				boost::property_tree::ptree::const_iterator mi = gi->second.begin(), me = gi->second.end();
				for (; mi != me; ++mi)
				{
					if (boost::algorithm::iequals( mi->first, "module"))
					{
						m_modules.push_back( utils::getCanonicalPath( mi->second.get_value<std::string>(), m_modulePath));
					}
				}
			}
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



