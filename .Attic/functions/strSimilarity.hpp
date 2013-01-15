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
//
// string similarity functions
//
#ifndef _STRING_SIMILARITY_HPP_INCLUDED
#define _STRING_SIMILARITY_HPP_INCLUDED

#include <string>

namespace _Wolframe {
namespace utils {

/// \brief Compute a degree of similarity between strings
/// \note This is work in progress and not used yet. Most likely it will also
/// go to a different place.
/// \param[in]	str1	string to be compared
/// \param[in]	str2	reference string
/// \returns		s distance between the normalized strings (0 => strings are equal)
int stringSimilarity( const std::string& str1, const std::string& str2 );

}} //namespace _Wolframe::utils

#endif // _STRING_SIMILARITY_HPP_INCLUDED
