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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file stringUtils.cpp
///\brief Some utility string function implementations.
/// They are mainly boost wrappers that are separated because of some warnings needed to be disabled

#ifdef _WIN32
#pragma warning(disable:4996)
#endif
#include "utils/miscUtils.hpp"
#include <cstring>
#include <sstream>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe::utils;

void _Wolframe::utils::splitString( std::vector<std::string>& res, const std::string& inp, const char* splitchrs)
{
	res.clear();
	std::vector<std::string> imm;
	boost::split( imm, inp, boost::is_any_of(splitchrs));
	std::vector<std::string>::const_iterator vi=imm.begin(), ve=imm.end();
	for (; vi != ve; ++vi) if (!vi->empty()) res.push_back( *vi);
}


/// Rules:
/// - trim the string left and right
/// - remove start and trailing punctuation marks
/// - replace punctuation marks with one space
/// - replace all whitespaces with one space
/// - set the string to uppercase.
void _Wolframe::utils::normalizeString( std::string& str )
{
	for ( std::string::iterator it = str.begin(); it != str.end(); it++ )	{
		if ( ispunct( *it ))
			*it++ = ' ';
	}
	boost::algorithm::trim( str );
	for ( std::string::iterator it = str.begin(); it != str.end(); it++ )	{
		if ( isspace( *it ))	{
			*it++ = ' ';
			while ( isspace( *it ) && it != str.end() )
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


/// A bit silly implementation maybe
/// Just normalize the strings and then compute a normalized Damerau–Levenshtein distance
/// \note The strings are truncated (after normalization) to 255 characters
int stringSimilarity( const std::string& str1, const std::string& str2 )
{
	static const int MaxStringSize = 255;
	static const int MaxInterval = 64;

	std::string ns1 = std::string( normalizeString_copy( str1 ), 0, MaxStringSize );
	std::string ns2 = std::string( normalizeString_copy( str2 ), 0, MaxStringSize );

	if ( ns1.empty() )	return ns2.length();
	if ( ns2.empty() )	return ns1.length();

	const char	*s1, *s2;
	int		l1, l2;
	if ( ns1.length() > ns2.length() )	{
		s1 = ns2.c_str(), l1 = ns2.length();
		s2 = ns1.c_str(), l2 = ns1.length();
	}
	else	{
		s1 = ns1.c_str(), l1 = ns1.length();
		s2 = ns2.c_str(), l2 = ns2.length();
	}

	int *currentRow = new int[ l1 + 1 ];
	int *previousRow = new int[ l1 + 1 ];
	int *transpositionRow = new int[ l1 + 1 ];

	for ( int i = 0; i <= l1; i++ )
		previousRow[ i ] = i;

	char s2LastChar = (char)0;
	for ( int i = 1; i <= l2; i++ )	{
		char s2Char = s2[ i - 1 ];
		currentRow[ 0 ] = i;

		int from = std::max( i - MaxInterval - 1, 1 );
		int to = std::min( i + MaxInterval + 1, l1 );

		char s1LastChar = (char)0;
		for ( int j = from; j <= to; j++ )	{
			char s1Char = s1[ j - 1 ];
			// Minimum price of the transition to the current state from the preceding one
			int cost = ( s1Char == s2Char ? 0 : 1 );
			int value = std::min( std::min( currentRow[ j - 1 ] + 1, previousRow[ j ] + 1 ),
					      previousRow[ j - 1 ] + cost );
			// Take into account also the transpozitions (Damerau)
			if ( s1Char == s2LastChar && s2Char == s1LastChar )
				value = std::min( value, transpositionRow[ j - 2 ] + cost );

			currentRow[ j ] = value;
			s1LastChar = s1Char;
		}
		s2LastChar = s2Char;

		int* tempRow = transpositionRow;
		transpositionRow = previousRow;
		previousRow = currentRow;
		currentRow = tempRow;
	}

	return previousRow[ l1 ];
}
