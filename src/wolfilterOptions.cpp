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
///\file wolfilterOptions.cpp
///\brief Implementation of the options of a wolfilter call
#include "wolfilterOptions.hpp"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>

using namespace _Wolframe;
using namespace config;

WolfilterOptions::WolfilterOptions( int argc, const char** argv)
	:m_help(false)
	,m_printversion(false)
{
	namespace po = boost::program_options;

	po::options_description fopt("Options");
	fopt.add_options()
		( "version,v", "print version" )
		( "help,h", "print help message" )
		( "input", po::value<std::string>(), "specify input file to process by path" )
		( "module", po::value< std::vector<std::string> >(), "specify module to load by path" )
		( "script", po::value< std::vector<std::string> >(), "specify script to load by path" )
		( "cmd", po::value<std::string>(), "name of the command to execute")
		( "input-filter", po::value<std::string>(), "specify input filter by name" )
		( "output-filter", po::value<std::string>(), "specify output filter by name" )
		;

	po::positional_options_description popt;
	popt.add( "cmd", 1);
	popt.add( "output-filter", 1);
	popt.add( "input-filter", 1);

	po::variables_map vmap;
	po::store( po::command_line_parser(argc, argv).options(fopt).positional(popt).run(), vmap);
	po::notify( vmap);

	m_printversion = vmap.count( "version");
	m_help = vmap.count( "help");

	if (vmap.count( "input")) m_inputfile = vmap["input"].as<std::string>();
	if (vmap.count( "module")) m_modules = vmap["module"].as<std::vector<std::string> >();
	if (vmap.count( "script")) m_scripts = vmap["script"].as<std::vector<std::string> >();
	if (vmap.count( "cmd")) m_cmd = vmap["cmd"].as<std::string>();
	if (vmap.count( "input-filter")) m_inputfilter = vmap["input-filter"].as<std::string>();
	if (vmap.count( "output-filter")) m_outputfilter = vmap["output-filter"].as<std::string>();

	if (m_outputfilter.empty()) m_outputfilter = m_inputfilter;		//... default same filter for input and output
	if (m_inputfilter.empty()) m_inputfilter = "xml:textwolf";		//... xml:textwolf as default input filter
	if (m_outputfilter.empty()) m_outputfilter = m_inputfilter;		//... default same filter for input and output

	std::ostringstream dd;
	dd << fopt;
	m_helpstring = dd.str();
}

void WolfilterOptions::print(std::ostream& out) const
{
	out << "Call:" << std::endl;
	out << "\twolfilter [OPTION] <cmd> <inputfilter> <outputfilter>" << std::endl;
	out << m_helpstring << std::endl;
}




