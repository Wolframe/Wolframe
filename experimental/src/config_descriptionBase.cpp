/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
#include <boost/property_tree/ptree.hpp>
#include <cstddef>

#include "structParser.hpp"
#include "structPrinter.hpp"
#include "descriptionBase.hpp"

using namespace _Wolframe::config;

bool DescriptionBase::parse( void* configStruct, const boost::property_tree::ptree& pt, std::string& errmsg) const
{
	try
	{
		try
		{
			std::vector<DescriptionBase::Item>::const_iterator itr,end;

			for (itr=m_ar.begin(),end=m_ar.end(); itr != end; ++itr)
			{
				itr->m_parse( itr->m_name.c_str(), configStruct, itr->m_ofs, pt);
			}
			return true;
		}
		catch (const ParseError& e)
		{
			errmsg.clear();
			errmsg.append( "parse error in configuration at element ");
			errmsg.append( e.m_location);
			errmsg.append( ": '");
			errmsg.append( e.m_message);
			errmsg.append( "'");
		}
	}
	catch (const std::exception& e)
	{
		errmsg = e.what();
	}
	return false;
}

void DescriptionBase::print( std::ostream& out, const char* name, const void* configStruct) const
{
	std::vector<Item>::const_iterator end = m_ar.end();
	std::vector<Item>::const_iterator itr = m_ar.begin();
	out << name << " {" << std::endl;

	while (itr != end)
	{
		itr->m_print( out, itr->m_name.c_str(), configStruct, itr->m_ofs, 1);
		itr++;
	}
	out << "}" << std::endl;
}


