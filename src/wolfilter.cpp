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
#include "langbind/iostreamfilter.hpp"

using namespace _Wolframe;

static void printUsage()
{
	std::cerr << "wolfilter <cmd> <inputfilter> [ <outputfilter> ]" << std::endl;
	std::cerr << "cmd          :Name of the command to execute ('-' for no command)" << std::endl;
	std::cerr << "inputfilter  :Name of the input filter plus an optional '/' plus buffer size" << std::endl;
	std::cerr << "outputfilter :Name of the output filter plus an optional '/' plus buffer size"<< std::endl;
	std::cerr << "              If not specified, then output filter equals input filter"  << std::endl  << std::endl;

	std::cerr << "   example: 'filter - xml:textwolf/256 xml:textwolf:UTF-8/128'" << std::endl;
	std::cerr << "      input = xml:textwolf/256 = using textwolf XML with a buffer of 256 bytes" << std::endl;
	std::cerr << "      output = xml:textwolf:UTF-16/128 = using textwolf XML with UTF-8 encoding and a buffer of 128 bytes" << std::endl;
}

int main( int argc, const char **argv )
{
	std::string infiltername;
	std::string outfiltername;
	std::string procname;
	std::size_t inputbufsize = 1024;
	std::size_t outputbufsize = 1024;

	if (argc == 4)
	{
		procname = argv[1];
		infiltername = argv[2];
		outfiltername = argv[3];
	}
	else if (argc == 3)
	{
		procname = argv[1];
		infiltername = argv[2];
		outfiltername = argv[2];
	}
	else if (argc > 4)
	{
		std::cerr << "too many arguments passed to " << argv[0] << std::endl;
		printUsage();
		return 1;
	}
	else if (argc < 3)
	{
		std::cerr << "too few arguments passed to " << argv[0] << std::endl;
		printUsage();
		return 2;
	}
	const char* bp;

	bp = std::strchr( infiltername.c_str(), '/');
	if (bp)
	{
		inputbufsize = (std::size_t)atoi( bp+1);
		infiltername.resize( bp-infiltername.c_str());
	}
	bp = std::strchr( outfiltername.c_str(), '/');
	if (bp)
	{
		outputbufsize = (std::size_t)atoi( bp+1);
		outfiltername.resize( bp-outfiltername.c_str());
	}
	if ((int)inputbufsize <= 0)
	{
		std::cerr << "illegal size of input buffer: " << (int)inputbufsize << std::endl;
	}
	if ((int)outputbufsize <= 0)
	{
		std::cerr << "illegal size of output buffer: " << (int)outputbufsize << std::endl;
	}
	if (!langbind::iostreamfilter( procname, infiltername, inputbufsize, outfiltername, outputbufsize, std::cin, std::cout))
	{
		std::cerr << "conversion error" << std::endl;
		return 3;
	}
	return 0;
}

