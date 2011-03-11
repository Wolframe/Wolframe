//
// database configuration functions
//


#include "standardConfigs.hpp"
#include "configHelpers.hpp"
#include "logger.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>


static const unsigned short DEFAULT_DB_CONNECTIONS = 4;


namespace _Wolframe	{
	namespace	Configuration	{

DatabaseConfiguration::DatabaseConfiguration() : ConfigurationBase( "Database Server" )
{
	port = 0;
	connections = 0;
	acquireTimeout = 0;
}


void DatabaseConfiguration::print( std::ostream& os ) const
{
	os << displayName() << std::endl;
	if ( host.empty())
		os << "   Database host: local unix domain socket" << std::endl;
	else
		os << "   Database host: " << host << ":" << port << std::endl;
	os << "   Database name: " << (name.empty() ? "(not specified - server user default)" : name) << std::endl;
	os << "   Database user: " << (user.empty() ? "(not specified - same as server user)" : user)
		 << ", password: " << (password.empty() ? "(not specified - no password used)" : password) << std::endl;
	os << "   Database connections: " << connections << std::endl;
	os << "   Acquire database connection timeout: " << acquireTimeout << std::endl;
}


/// Check if the database configuration makes sense
bool DatabaseConfiguration::check() const
{
	if ( connections == 0 )	{
		LOG_ERROR << "Invalid number of connections: " << connections;
		return false;
	}
	return true;
}


bool DatabaseConfiguration::parse( const boost::property_tree::ptree& pt, const std::string& /* nodeName */, std::ostream& os )
{
	for ( boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++ )	{
		if ( boost::algorithm::iequals( it->first, "host" ))	{
			if ( !getStringValue( it, displayName(), "host", host, os ))		return false;
		}
		else if ( boost::algorithm::iequals( it->first, "port" ))	{
			if ( !getUnsignedShortValue( it, displayName(), "port", port, os ))	return false;
		}
		else if ( boost::algorithm::iequals( it->first, "name" ))	{
			if ( !getStringValue( it, displayName(), "name", name, os ))		return false;
		}
		else if ( boost::algorithm::iequals( it->first, "user" ))	{
			if ( !getStringValue( it, displayName(), "user", user, os ))		return false;
		}
		else if ( boost::algorithm::iequals( it->first, "password" ))	{
			if ( !getStringValue( it, displayName(), "password", password, os ))	return false;
		}
		else if ( boost::algorithm::iequals( it->first, "connections" ))	{
			if ( !getUnsignedShortValue( it, displayName(), "connections", connections, os ))
												return false;
		}
		else if ( boost::algorithm::iequals( it->first, "acquireTimeout" ))	{
			if ( !getUnsignedShortValue( it, displayName(), "acquireTimeout", acquireTimeout, os ))
												return false;
		}
		else	{
			LOG_WARNING << displayName() << ": unknown configuration option: <" << it->first << ">";
//			return false;
		}
	}
	if ( connections == 0 )
		connections = DEFAULT_DB_CONNECTIONS;

	return true;
}


	} // namespace Configuration
} // namespace _Wolframe
