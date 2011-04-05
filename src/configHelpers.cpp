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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/

#include "configHelpers.hpp"
#include "logger.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>

namespace _Wolframe	{
	namespace	Configuration	{


bool getBoolValue( const boost::property_tree::ptree& pt, const std::string& name,
		   const std::string& module, bool& value )
{
	bool	dummy = false;
	return getBoolValue( pt, name, module, value, dummy );
}


bool getBoolValue( const boost::property_tree::ptree& pt, const std::string& name,
		   const std::string& module, bool& value, bool& valueIsSet )
{
	std::string s = pt.get_value<std::string>();
	boost::to_upper( s );
	boost::trim( s );
	if ( valueIsSet )	{
		LOG_ERROR << module << ": " << name << " redefined";
		return false;
	}
	if ( s == "NO" || s == "FALSE" || s == "0" || s == "OFF" )	{
		value = false;
		valueIsSet = true;
		return true;
	}
	if ( s == "YES" || s == "TRUE" || s == "1" || s == "ON" )	{
		value = true;
		valueIsSet = true;
		return true;
	}
	LOG_ERROR << module << ": invalid logical value for " << name << ": \""
		  << pt.get_value<std::string>() << "\"";
	return false;
}


bool getStringValue( const boost::property_tree::ptree& pt, const std::string& name,
		     const std::string& module, std::string& value )
{
	if ( !value.empty() )	{
		LOG_ERROR << module << ": " << name << " redefined";
		return false;
	}
	value = pt.get_value<std::string>();
	if ( value.empty() )	{
		LOG_ERROR << module << ": invalid value for " << name << ": \""
			  << pt.get_value<std::string>() << "\"";
		return false;
	}
	return true;
}


bool getHostnameValue( const boost::property_tree::ptree& pt, const std::string& name,
		       const std::string& module, std::string& value )
{
	bool ret = getStringValue( pt, name, module, value );
	if ( ret && value == "*" )
		value = "0.0.0.0";
	return ret;
}

	} // namespace Configuration
} // namespace _Wolframe
