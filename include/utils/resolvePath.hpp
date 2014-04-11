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
///\file utils/resolvePath.hpp
///\brief Functions for resolving relative pathes

#ifndef _WOLFRAME_UTILS_RESOLVE_PATH_HPP_INCLUDED
#define _WOLFRAME_UTILS_RESOLVE_PATH_HPP_INCLUDED
#include <string>

namespace _Wolframe {
namespace utils {

///\brief Remove .. and . from a path
///\param[in] path Path to process
std::string resolvePath( const std::string& path );

///\brief Get the path 'path' relative to 'refpath' if it is not absolute
///\param[in] path relative part of the path or absolute path
///\param[in] refPath absolute part of the path if 'path' is relative
///\return the canonical path
std::string getCanonicalPath( const std::string& path, const std::string& refPath);

}}//namespace
#endif

