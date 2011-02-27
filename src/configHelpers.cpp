//
// helpers for the configuration functions
//


#include "configHelpers.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>

namespace _Wolframe	{
	namespace	Configuration	{


bool getBoolValue( boost::property_tree::ptree::const_iterator it, const std::string& module,
		   const std::string& name, bool& value, std::ostream& os )
{
	std::string s = it->second.get_value<std::string>();
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
	os << module << ": invalid logical value for " << name << ": <"	<< it->second.get_value<std::string>() << ">";
	return false;
}


bool getStringValue( boost::property_tree::ptree::const_iterator it, const std::string& module,
		     const std::string& name, std::string& value, std::ostream& os )
{
	if ( !value.empty() )	{
		os << module << ": " << name << " redefined";
		return false;
	}
	value = it->second.get_value<std::string>();
	if ( value.empty() )	{
		os << module << ": invalid " << name << ": <" << it->second.get_value<std::string>() << ">";
		return false;
	}
	return true;
}


bool getHostnameValue( boost::property_tree::ptree::const_iterator it, const std::string& module,
		     const std::string& name, std::string& value, std::ostream& os )
{
	if ( !value.empty() )	{
		os << module << ": " << name << " redefined";
		return false;
	}
	value = it->second.get_value<std::string>();
	if ( value.empty() )	{
		os << module << ": invalid " << name << ": <" << it->second.get_value<std::string>() << ">";
		return false;
	}
	if ( value == "*" )
		value = "0.0.0.0";
	return true;
}


bool getUnsignedShortValue( boost::property_tree::ptree::const_iterator it, const std::string& module,
			    const std::string& name, unsigned short& value, std::ostream& os )
{
	if ( value != 0 )	{
		os << module << ": " << name << " redefined";
		return false;
	}
	value = it->second.get_value<unsigned short>();
	if ( value == 0 )	{
		os << module << ": invalid " << name << ": <" << it->second.get_value<std::string>() << ">";
		return false;
	}
	return true;
}


	} // namespace Configuration
} // namespace _Wolframe
