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
///\file pdfPrinterMethod.cpp
#include "pdfPrinterMethod.hpp"
#include <stdexcept>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::prnt;

const char* _Wolframe::prnt::methodName( Method::Id m)
{
	static const char* ar[] = {
		"AddPage",
		"PrintText",
		"DrawRectangle",
		"DrawLine",
		0};
	return ar[ (int)m];
}

namespace {
struct MethodnameMap :public std::map <std::string, std::size_t>
{
	static std::string unifyKey( const std::string& key)
	{
		return boost::algorithm::to_lower_copy(key);
	}

	MethodnameMap()
	{
		for (std::size_t ii=0; methodName( (Method::Id)ii); ++ii)
		{
			(*this)[ unifyKey( methodName( (Method::Id)ii))] = ii;
		}
	}
};
}//anonymous namespace

Method::Id _Wolframe::prnt::methodId( const std::string& name)
{
	static MethodnameMap map;
	std::string key( map.unifyKey( name));
	std::map <std::string, std::size_t>::const_iterator itr = map.find( key);
	if (itr == map.end()) throw std::runtime_error( std::string( "unknown method '") + name + "'");
	return (Method::Id)itr->second;
}


