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
//
// daemon / service configuration
//

#include "standardConfigs.hpp"
#include "configHelpers.hpp"
#include "appProperties.hpp"
#include "logger.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>

namespace _Wolframe {
namespace config {

// Constructor
#if !defined(_WIN32)	// Unix daemon
ServiceConfiguration::ServiceConfiguration() : ConfigurationBase( "Daemon" )	{}
#else
ServiceConfiguration::ServiceConfiguration() : ConfigurationBase( "Service" )	{}
#endif


// Server configuration functions
void ServiceConfiguration::print( std::ostream& os ) const
{
	os << displayName() << std::endl;
#if !defined(_WIN32)	// Unix daemon
	os << "   Run as " << (user.empty() ? "(not specified)" : user) << ":"
	   << (group.empty() ? "(not specified)" : group) << std::endl;
	os << "   PID file: " << pidFile << std::endl;
#else
	// Windows service
	os << "   When run as service" << std::endl
	   << "      Name: " << serviceName << std::endl
	   << "      Displayed name: " << serviceDisplayName << std::endl
	   << "      Description: " << serviceDescription << std::endl;
#endif
}


/// Check if the server configuration makes sense
bool ServiceConfiguration::check() const
{
	return true;
}


/// Parse the configuration
bool ServiceConfiguration::parse( const boost::property_tree::ptree& pt,
				  const std::string& node )
{
#if defined(_WIN32)
	if ( boost::algorithm::iequals( node, "daemon" ))	{
		LOG_WARNING << "daemon: section is not valid on Windows";
	}
#else // #if defined(_WIN32)
	if ( boost::algorithm::iequals( node, "daemon" ))	{
		for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
			if ( boost::algorithm::iequals( L1it->first, "user" ))	{
				if ( ! getStringValue( L1it->second, L1it->first, displayName(), user ))
					return false;
			}
			else if ( boost::algorithm::iequals( L1it->first, "group" ))	{
				if ( ! getStringValue( L1it->second, L1it->first, displayName(), group ))
					return false;
			}
			else if ( boost::algorithm::iequals( L1it->first, "pidFile" ))	{
				if ( ! getStringValue( L1it->second, L1it->first, displayName(), pidFile ))
					return false;
				if ( ! boost::filesystem::path( pidFile ).is_absolute() )
					LOG_WARNING << displayName() << ": pid file path is not absolute: "
						    << pidFile;
			}
			else	{
				LOG_WARNING << displayName() << ": unknown configuration option: <"
					    << L1it->first << ">";
			}
		}
	}
#endif
#if !defined(_WIN32)
	else if ( boost::algorithm::iequals( node, "service" ))	{
		LOG_WARNING << "service: section is valid only on Windows";
	}
#else // #if defined(_WIN32)
	else if ( boost::algorithm::iequals( node, "service" ))	{
		for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
			if ( boost::algorithm::iequals( L1it->first, "serviceName" ))	{
				if ( ! getStringValue( L1it->second, L1it->first, displayName(), serviceName ))
					return false;
			}
			else if ( boost::algorithm::iequals( L1it->first, "displayName" ))	{
				if ( ! getStringValue( L1it->second, L1it->first, displayName(), serviceDisplayName ))
					return false;
			}
			else if ( boost::algorithm::iequals( L1it->first, "description" ))	{
				if ( ! getStringValue( L1it->second, L1it->first, displayName(), serviceDescription ))
					return false;
			}
			else	{
				LOG_WARNING << displayName() << ": unknown configuration option: <"
					    << L1it->first << ">";
			}
		}
		if ( serviceName.empty() )
			serviceName = defaultServiceName();
		if ( serviceDisplayName.empty() )
			serviceDisplayName = defaultServiceDisplayName();
		if ( serviceDescription.empty() )
			serviceDescription = defaultServiceDescription();
	}
#endif
	else	{
		LOG_WARNING << displayName() << ": unknown configuration option: <" << node << ">";
	}
	return true;
}


#if !defined(_WIN32)
void ServiceConfiguration::setCanonicalPathes( const std::string& refPath )
{
	if ( ! pidFile.empty() )	{
		if ( ! boost::filesystem::path( pidFile ).is_absolute() )
			pidFile = resolvePath( boost::filesystem::absolute( pidFile,
									    boost::filesystem::path( refPath ).branch_path()).string());
		else
			pidFile = resolvePath( pidFile );
	}
}


/// Override the server configuration with command line arguments
void ServiceConfiguration::override( const std::string& usr, const std::string& grp )
{
	if ( !usr.empty())
		user = usr;
	if ( !grp.empty())
		group = grp;
}
#endif // !defined(_WIN32)

}} // namespace _Wolframe::config

