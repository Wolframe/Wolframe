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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file strNormalization.cpp
///\brief string normalization functions implementations.
///

#include <string>
#include <cctype>
#include <boost/algorithm/string.hpp>
#include "utils/strNormalization.hpp"

using namespace _Wolframe::utils;

/// Rules:
/// - trim the string left and right
/// - remove start and trailing punctuation marks
/// - replace punctuation marks with one space
/// - replace all whitespaces with one space
/// - set the string to uppercase.
void _Wolframe::utils::normalizeString( std::string& str )
{
	for ( std::string::iterator it = str.begin(); it != str.end(); it++ )	{
		if ( ((unsigned char)*it < 128) && ispunct( *it ))
			*it++ = ' ';
	}
	boost::algorithm::trim( str );
	for ( std::string::iterator it = str.begin(); it != str.end(); it++ )	{
		if ( ((unsigned char)*it < 128) && isspace( *it ))	{
			*it++ = ' ';
			while ( it != str.end() && ((unsigned char)*it < 128) && isspace( *it ) )
				str.erase( it );
		}
	}
	boost::algorithm::to_upper( str );
}

/// Rules are the same as for normalizeString(...)
std::string _Wolframe::utils::normalizeString_copy( const std::string& str )
{
	std::string ret = str;
	normalizeString( ret );
	return ret;
}
