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


bool DatabaseConfiguration::parse( const boost::property_tree::ptree::const_iterator it,
				   const std::string& /* nodeName */ )
{
	for ( boost::property_tree::ptree::const_iterator L1it = it->second.begin();
							L1it != it->second.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "host" ))	{
			if ( !getStringValue( L1it, displayName(), host ))		return false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "port" ))	{
			if ( !getUnsignedShortValue( L1it, displayName(), port ))	return false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "name" ))	{
			if ( !getStringValue( L1it, displayName(), name ))		return false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "user" ))	{
			if ( !getStringValue( L1it, displayName(), user ))		return false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "password" ))	{
			if ( !getStringValue( L1it, displayName(), password ))		return false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "connections" ))	{
			if ( !getUnsignedShortValue( L1it, displayName(), connections ))
											return false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "acquireTimeout" ))	{
			if ( !getUnsignedShortValue( L1it, displayName(), acquireTimeout ))
											return false;
		}
		else	{
			LOG_WARNING << displayName() << ": unknown configuration option: <"
				    << L1it->first << ">";
//			return false;
		}
	}
	if ( connections == 0 )
		connections = DEFAULT_DB_CONNECTIONS;

	return true;
}

	} // namespace Configuration
} // namespace _Wolframe
