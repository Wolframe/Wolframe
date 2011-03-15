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


namespace _Wolframe	{
	namespace Configuration	{

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
bool ServiceConfiguration::parse( const boost::property_tree::ptree& pt, const std::string& node )
{
#if defined(_WIN32)
	if ( boost::algorithm::iequals( node, "daemon" ))	{
		LOG_WARNING << "daemon: section is not valid on Windows" << std::endl;
	}
#else // #if defined(_WIN32)
	if ( boost::algorithm::iequals( node, "daemon" ))	{
		for ( boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++ )	{
			if ( boost::algorithm::iequals( it->first, "user" ))	{
				if ( ! getStringValue( it->second, displayName(), "user", user ))
					return false;
			}
			else if ( boost::algorithm::iequals( it->first, "group" ))	{
				if ( ! getStringValue( it->second, displayName(), "group", group ))
					return false;
			}
			else if ( boost::algorithm::iequals( it->first, "pidFile" ))	{
				if ( ! getStringValue( it->second, displayName(), "pidFile", pidFile ))
					return false;
				if ( ! boost::filesystem::path( pidFile ).is_absolute() )
					LOG_WARNING << displayName() << ": pid file path is not absolute: "
									   << pidFile << std::endl;
			}
			else	{
				LOG_WARNING << displayName() << ": unknown configuration option: <" << it->first << ">";
//				return false;
			}
		}
	}
#endif
#if !defined(_WIN32)
	else if ( boost::algorithm::iequals( node, "service" ))	{
		LOG_WARNING << "service: section is valid only on Windows" << std::endl;
	}
#else // #if defined(_WIN32)
	else if ( boost::algorithm::iequals( node, "service" ))	{
		for ( boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++ )	{
			if ( boost::algorithm::iequals( it->first, "serviceName" ))	{
				if ( ! getStringValue( it->second, displayName(), "serviceName", serviceName ))
					return false;
			}
			else if ( boost::algorithm::iequals( it->first, "displayName" ))	{
				if ( ! getStringValue( it->second, displayName(), "displayName", serviceDisplayName ))
					return false;
			}
			else if ( boost::algorithm::iequals( it->first, "description" ))	{
				if ( ! getStringValue( it->second, displayName(), "description", serviceDescription ))
					return false;
			}
			else	{
				LOG_WARNING << displayName() << ": unknown configuration option: <" << it->first << ">";
//				return false;
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
//		return false;
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

	} // namespace Configuration
} // namespace _Wolframe

