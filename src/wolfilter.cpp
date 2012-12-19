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
///\brief Program using wolframe functions to map stdin to stdout
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include "langbind/iostreamfilter.hpp"
#include "prgbind/predefinedProgramTypes.hpp"
#include "wolfilterCommandLine.hpp"
#include "moduleInterface.hpp"
#include "processor/procProvider.hpp"
#include <boost/filesystem.hpp>

using namespace _Wolframe;

///\TODO Not to be defined here
static const unsigned short APP_MAJOR_VERSION = 0;
static const unsigned short APP_MINOR_VERSION = 0;
static const unsigned short APP_REVISION = 5;
static const unsigned short APP_BUILD = 0;

int main( int argc, char **argv )
{
	bool doExit = false;
	try
	{
		static boost::filesystem::path execdir = boost::filesystem::system_complete( argv[0]).parent_path();
		module::ModulesDirectory modDir;
		config::WolfilterCommandLine cmdline( argc, argv, execdir.string());
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

		// Load the modules, scripts, etc. defined in the command line into the global context:
		db::DatabaseProvider databaseProvider( &cmdline.dbProviderConfig(), &cmdline.modulesDirectory());
		proc::ProcessorProvider processorProvider( &cmdline.procProviderConfig(), &cmdline.modulesDirectory(), prgbind::getPredefinedProgramTypes());
		processorProvider.resolveDB( databaseProvider);

		// Call the function to execute
		if (cmdline.inputfile().size())
		{
			std::fstream fh;
			fh.exceptions( std::ifstream::failbit | std::ifstream::badbit);
			fh.open( cmdline.inputfile().c_str(), std::ios::in | std::ios::binary);

			langbind::iostreamfilter( &processorProvider, cmdline.cmd(), cmdline.inputfilter(), cmdline.inbufsize(), cmdline.outputfilter(), cmdline.outbufsize(), fh, std::cout);
		}
		else
		{
			langbind::iostreamfilter( &processorProvider, cmdline.cmd(), cmdline.inputfilter(), cmdline.inbufsize(), cmdline.outputfilter(), cmdline.outbufsize(), std::cin, std::cout);
		}
	}
	catch (const std::bad_alloc& e)
	{
		std::cerr << "out of memory in wolfilter" << std::endl;
		return 1;
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		return 2;
	}
	catch (const std::exception& e)
	{
		std::cerr << "exception " << e.what() << std::endl;
		return 2;
	}
	return 0;
}

