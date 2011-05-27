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

#include "config/structParser.hpp"
#include "config/descriptionBase.hpp"

using namespace _Wolframe::config;

bool DescriptionBase::parse( void* configStruct, const boost::property_tree::ptree& pt, std::string& errmsg) const
{
	try
	{
		try
		{
			unsigned int ii,nn=m_ar.size();
			for (ii=0; ii<nn; ii++)
			{
				boost::property_tree::ptree::const_iterator end = pt.end();
				for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != end; ++it)
				{
					if (boost::iequals( it->first, m_ar[ii].m_name))
					{
						m_ar[ii].m_parse( m_ar[ii].m_name.c_str(), configStruct, m_ar[ii].m_ofs, it->second);
					}
				}
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

