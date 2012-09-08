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
///\file filter_filter.cpp
///\brief Implementation of filter class
#include "filter/filter.hpp"
#include <cstring>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

bool Filter::getValue( const char* name, std::string& val) const
{
	if (m_inputfilter.get() && m_inputfilter->getValue( name, val)) return true;
	if (m_outputfilter.get() && m_outputfilter->getValue( name, val)) return true;
	return false;
}

std::pair<std::string,std::string> Filter::identifier( const std::string& id)
{
	const char* cc = id.c_str();
	const char* ee = std::strchr( cc, ';');
	if (ee)
	{
		return std::pair<std::string,std::string>( std::string( cc, ee-cc), ee + 1);
	}
	else
	{
		return std::pair<std::string,std::string>( cc, "");
	}
}

bool Filter::setValue( const char* name, const std::string& value)
{
	bool rt = false;
	if (m_inputfilter.get() && m_inputfilter->setValue( name, value)) rt = true;
	if (m_outputfilter.get() && m_outputfilter->setValue( name, value)) rt = true;
	return rt;
}

