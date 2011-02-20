//
// database configuration functions
//


#include "configStandard.hpp"
#include "configHelpers.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>


namespace _SMERP	{
	namespace	Configuration	{


void DatabaseConfiguration::print( std::ostream& os ) const
{
	os << displayStr() << std::endl;
	if ( host.empty())
		os << "   DB host: local unix domain socket" << std::endl;
	else
		os << "   DB host: " << host << ":" << port << std::endl;
	os << "   DB name: " << (name.empty() ? "(not specified - server user default)" : name) << std::endl;
	os << "   DB user / password: " << (user.empty() ? "(not specified - same as server user)" : user) << " / "
			<< (password.empty() ? "(not specified - no password used)" : password) << std::endl;
}


/// Check if the database configuration makes sense
bool DatabaseConfiguration::check( std::ostream& ) const
{
	return true;
}


bool DatabaseConfiguration::parse( boost::property_tree::ptree& pt, std::ostream& os )
{
	for ( boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++ )	{
		if ( boost::algorithm::iequals( it->first, "host" ))	{
			if ( !getStringValue( it, displayStr(), "host", host, os ))		return false;
		}
		else if ( boost::algorithm::iequals( it->first, "port" ))	{
			if ( !getUnsignedShortValue( it, displayStr(), "port", port, os ))	return false;
		}
		else if ( boost::algorithm::iequals( it->first, "name" ))	{
			if ( !getStringValue( it, displayStr(), "name", name, os ))		return false;
		}
		else if ( boost::algorithm::iequals( it->first, "user" ))	{
			if ( !getStringValue( it, displayStr(), "user", user, os ))		return false;
		}
		else if ( boost::algorithm::iequals( it->first, "password" ))	{
			if ( !getStringValue( it, displayStr(), "password", password, os ))	return false;
		}
		else	{
			os << displayStr() << ": unknown configuration option: <" << it->first << ">";
			return false;
		}
	}
	return true;
}


	} // namespace Configuration
} // namespace _SMERP
