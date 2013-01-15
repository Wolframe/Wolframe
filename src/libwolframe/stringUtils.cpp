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
#include <boost/algorithm/string.hpp>
#include <boost/range/iterator_range.hpp>

using namespace _Wolframe::utils;

void _Wolframe::utils::splitString( std::vector<std::string>& res, const std::string& inp, const char* splitchrs)
{
	res.clear();
	std::vector<std::string> imm;
	boost::split( imm, inp, boost::is_any_of(splitchrs));
	std::vector<std::string>::const_iterator vi=imm.begin(), ve=imm.end();
	for (; vi != ve; ++vi) if (!vi->empty()) res.push_back( *vi);
}

void _Wolframe::utils::splitString( std::vector<std::string>& res, std::string::const_iterator begin, std::string::const_iterator end, const char* splitchrs)
{
	res.clear();
	std::vector<std::string> imm;
	typedef boost::iterator_range<std::string::const_iterator> irange;
	irange ir = boost::make_iterator_range( begin, end);
	boost::split( imm, ir, boost::is_any_of(splitchrs));
	std::vector<std::string>::const_iterator vi=imm.begin(), ve=imm.end();
	for (; vi != ve; ++vi) if (!vi->empty()) res.push_back( *vi);
}
