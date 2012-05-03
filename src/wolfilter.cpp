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
	std::cerr << "filter <inputfilter> [ <command> ] <outputfilter>" << std::endl;
	std::cerr << "inputfilter :Name of the input filter plus an optional '/' plus buffer size" << std::endl;
	std::cerr << "outputfilter :Name of the output filter plus an optional '/' plus buffer size" << std::endl << std::endl;

	std::cerr << "   example: 'filter xml:textwolf/256 xml:textwolf:UTF-8/128'" << std::endl;
	std::cerr << "      input = xml:textwolf/256 = using textwolf XML with a buffer of 256 bytes" << std::endl;
	std::cerr << "      output = xml:textwolf:UTF-16/128 = using textwolf XML with UTF-8 encoding and a buffer of 128 bytes" << std::endl;
}

int main( int argc, const char **argv )
{
	const char* infiltername = 0;
	const char* outfiltername = 0;
	const char* procname = 0;
	std::size_t inputBufferSize = 1024;
	std::size_t outputBufferSize = 1024;

	if (argc == 4)
	{
		infiltername = argv[1];
		procname = argv[2];
		outfiltername = argv[3];
	}
	else if (argc == 3)
	{
		infiltername = argv[1];
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
		std::cerr << "too many arguments passed to " << argv[0] << std::endl;
		printUsage();
		return 2;
	}
	std::string filternameIn( infiltername);
	std::string filternameOut( outfiltername);
	const char* bp;

	bp = std::strchr( filternameIn.c_str(), '/');
	if (bp)
	{
		inputBufferSize = (std::size_t)atoi( bp+1);
		filternameIn.resize( bp-filternameIn.c_str());
	}
	bp = std::strchr( filternameOut.c_str(), '/');
	if (bp)
	{
		outputBufferSize = (std::size_t)atoi( bp+1);
		filternameOut.resize( bp-filternameOut.c_str());
	}
	if (procname)
	{
		return langbind::iostreamfilter( procname, filternameIn.c_str(), inputBufferSize, filternameOut.c_str(), outputBufferSize, std::cin, std::cout);
	}
	else
	{
		return langbind::iostreamfilter( filternameIn.c_str(), inputBufferSize, filternameOut.c_str(), outputBufferSize, std::cin, std::cout);
	}
}

