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
///\brief Program using filters to map stdin to stdout
#include "wtest/testDescription.hpp"
#include <stdexcept>
#include <iostream>

using namespace _Wolframe;
using namespace wtest;

static void printUsage()
{
	std::cerr << "wtestcheck <path>" << std::endl;
	std::cerr << "path         :Path of the test description file to check" << std::endl;
}

int main( int argc, const char **argv )
{
	if (argc > 2)
	{
		std::cerr << "too many arguments passed to " << argv[0] << std::endl;
		printUsage();
		return 1;
	}
	else if (argc < 2)
	{
		std::cerr << "too few arguments passed to " << argv[0] << std::endl;
		printUsage();
		return 2;
	}
	try
	{
		TestDescription test( argv[1], argv[0]);
	}
	catch (const std::exception& e)
	{
		std::cerr << "test validation failed: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}

