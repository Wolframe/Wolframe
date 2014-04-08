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
//\file utils/fileLineInfo.cpp
//\brief Implementation of positional info in files for error messages
#include "utils/fileLineInfo.hpp"
#include <cstring>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::utils;

FileLineInfo::FileName utils::FileLineInfo::getFileName( const std::string& name)
{
	char* cc = (char*)std::malloc( name.size()+1);
	if (!cc) throw std::bad_alloc();
	std::memcpy( cc, name.c_str(), name.size()+1);
	return boost::shared_ptr<char>( cc, std::free);
}

std::string FileLineInfo::logtext() const
{
	if (!filename()) return std::string();
	return std::string("in file '") + filename() + "' at line " + boost::lexical_cast<std::string>(line()) + " column " + boost::lexical_cast<std::string>(column());
}

