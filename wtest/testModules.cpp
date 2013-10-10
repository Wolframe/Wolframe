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
///\file wtest/testModules.cpp
///\brief Implements a function to get list of modules available for a test program
#include "wtest/testModules.hpp"
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::wtest;
 
std::list<std::string> _Wolframe::wtest::getTestModuleList( const std::string& topdir)
{
	std::list<std::string> rt;
	static const char* ar[] = {
#if WITH_TEXTWOLF
		"filter/textwolf/mod_filter_textwolf",
		"filter/char/mod_filter_char",
		"filter/line/mod_filter_line",
#endif
#if WITH_LIBXML2
		"filter/libxml2/mod_filter_libxml2",
#endif
#if WITH_LUA
		"cmdbind/lua/mod_command_lua",
#endif
#if WITH_PYTHON
		"cmdbind/python/mod_command_python",
#endif
		"cmdbind/directmap/mod_command_directmap",
		0};
	std::size_t ii = 0;
	while (ar[ii])
	{
		boost::filesystem::path pt( topdir);
		pt /= "src";
		pt /= "modules";
		pt /= ar[ii++];
		rt.push_back( pt.string());
	}
	return rt;
}


