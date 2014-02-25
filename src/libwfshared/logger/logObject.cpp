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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file logger/logObject.cpp
///\brief Implements uniform handling of logger and exception objects
#include "logger/logObject.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::log;

LogObjectDescriptionBase::LogObjectDescriptionBase( const std::string& formatstring)
{
	m_strings.push_back('\0');
	std::string::const_iterator mk = formatstring.begin(), si = formatstring.begin(), se = formatstring.end();
	for (; si != se; ++si)
	{
		if (*si == '$')
		{
			FormatElement str;
			str.index = m_strings.size();
			m_strings.append( std::string( mk, si));
			m_strings.push_back('\0');
			m_format.push_back( str);
			++si;
			mk = si;
			if (*si == '$') continue;
			std::string idxstr;
			for (; si != se && *si >= '0' && *si <= '9'; ++si) idxstr.push_back( *si);
			if (idxstr.empty()) throw std::logic_error( "format string reference is not a number");
			m_format.push_back( FormatElement( boost::lexical_cast<std::size_t>( idxstr)));
			if (si != se)
			{
				if ((*si >= 'a' && *si <= 'z') || (*si >= 'A' && *si <= 'Z') || *si == '_')
				{
					throw std::logic_error( "format string reference is immediately followed by an identifier");
				}
			}
			mk = si;
			--si;
		}
	}
	if (mk != se)
	{
		FormatElement str;
		str.index = m_strings.size();
		m_strings.append( std::string( mk, se));
		m_strings.push_back('\0');
		m_format.push_back( str);
	}
}

void LogObjectDescriptionBase::substitute( std::size_t idx, ToStringMethod tostring, std::size_t ofs)
{
	bool match = false;
	std::vector<FormatElement>::iterator si = m_format.begin(), se = m_format.end();
	for (; si != se; ++si)
	{
		if (si->type == FormatElement::Index && si->index == idx)
		{
			si->type = FormatElement::Reference;
			si->tostring = tostring;
			si->index = ofs;
			match = true;
		}
	}
	if (!match) throw std::logic_error( "log object bind index not found or duplicated substitution");
}

void LogObjectDescriptionBase::checkUnresolved()
{
	std::vector<FormatElement>::iterator si = m_format.begin(), se = m_format.end();
	for (; si != se; ++si)
	{
		if (si->type == FormatElement::Index) throw std::logic_error( "unresolved object reference");
	}
}

std::string LogObjectDescriptionBase::objToString( const void* obj) const
{
	std::string rt;
	std::vector<FormatElement>::const_iterator si = m_format.begin(), se = m_format.end();
	for (; si != se; ++si)
	{
		if (si->type == FormatElement::String)
		{
			if (si->index > m_strings.size()) throw std::logic_error("internal: corrupt log object");
			rt.append( m_strings.c_str()+si->index);
		}
		else if (si->type == FormatElement::Reference)
		{
			rt.append( si->tostring( (const void*)((const char*)obj + si->index)));
		}
		else
		{
			std::logic_error( "unresolved object reference");
		}
	}
	return rt;
}

