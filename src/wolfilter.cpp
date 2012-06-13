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
///\brief Program using filters to map stdin to stdout
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include "langbind/appGlobalContext.hpp"
#include "langbind/iostreamfilter.hpp"
#include "wolfilterCommandLine.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;

///\TODO Not to be defined here
static const unsigned short APP_MAJOR_VERSION = 0;
static const unsigned short APP_MINOR_VERSION = 0;
static const unsigned short APP_REVISION = 5;
static const unsigned short APP_BUILD = 0;

///\brief Loads the modules, scripts, etc. defined in the command line into the global context
static void loadGlobalContext( const config::WolfilterCommandLine& cmdline)
{
	langbind::GlobalContext* gct = langbind::getGlobalContext();

	std::vector<std::string>::const_iterator itr = cmdline.scripts().begin(), end = cmdline.scripts().end();
	for (; itr != end; ++itr)
	{
		boost::filesystem::path scriptpath( boost::filesystem::current_path() / "temp" / *itr);
		langbind::LuaScript script( scriptpath.string());
		std::vector<std::string>::const_iterator fi = script.functions().begin(), fe = script.functions().end();
		for (; fi != fe; ++fi)
		{
			gct->defineLuaFunction( *fi, script);
		}
	}
}

int main( int argc, const char **argv )
{
	bool doExit = false;
	try
	{
		config::WolfilterCommandLine cmdline( argc, argv);
		if (cmdline.printversion())
		{
			std::cerr << "wolfilter version ";
			std::cerr << APP_MAJOR_VERSION << "." << APP_MINOR_VERSION << "." << APP_REVISION << "." << APP_BUILD << std::endl;
			doExit = true;
		}
		if (cmdline.printhelp())
		{
			cmdline.print( std::cerr);
			doExit = true;
		}
		if (doExit) return 0;

		loadGlobalContext( cmdline);

		if (!langbind::iostreamfilter( cmdline.cmd(), cmdline.inputfilter(), cmdline.inbufsize(), cmdline.outputfilter(), cmdline.outbufsize(), std::cin, std::cout))
		{
			std::cerr << "conversion error" << std::endl;
			return -3;
		}
	}
	catch (const std::bad_alloc& e)
	{
		std::cerr << "out of memory in wolfilter" << std::endl;
		return -1;
	}
	catch (const std::exception& e)
	{
		std::cerr << "error in wolfilter: " << e.what() << std::endl;
		return -2;
	}
	return 0;
}

