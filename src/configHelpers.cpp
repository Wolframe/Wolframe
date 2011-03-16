//
// helpers for the configuration functions
//

#include "configHelpers.hpp"
#include "logger.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>

namespace _Wolframe	{
	namespace	Configuration	{


bool getBoolValue( const boost::property_tree::ptree& pt, const std::string& module,
		   const std::string& name, bool& value )
{
	std::string s = pt.get_value<std::string>();
	boost::to_upper( s );
	boost::trim( s );
	if ( s == "NO" || s == "FALSE" || s == "0" || s == "OFF" )	{
		value = false;
		return true;
	}
	if ( s == "YES" || s == "TRUE" || s == "1" || s == "ON" )	{
		value = true;
		return true;
	}
	LOG_ERROR << module << ": invalid logical value for " << name << ": \""
		  << pt.get_value<std::string>() << "\"";
	return false;
}


bool getStringValue( const boost::property_tree::ptree& pt, const std::string& module,
		     const std::string& name, std::string& value )
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


bool getHostnameValue( const boost::property_tree::ptree& pt, const std::string& module,
		     const std::string& name, std::string& value )
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
	if ( value == "*" )
		value = "0.0.0.0";
	return true;
}


bool getUnsignedShortValue( const boost::property_tree::ptree& pt, const std::string& module,
			    const std::string& name, unsigned short& value )
{
	if ( value != 0 )	{
		LOG_ERROR << module << ": " << name << " redefined";
		return false;
	}
	value = pt.get_value<unsigned short>();
	if ( value == 0 )	{
		LOG_ERROR << module << ": invalid value for " << name << ": \""
			  << pt.get_value<std::string>() << "\"";
		return false;
	}
	return true;
}


	} // namespace Configuration
} // namespace _Wolframe
