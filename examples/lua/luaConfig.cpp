//
// lua configuration functions
//

#include "handlerConfig.hpp"
#include "configHelpers.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>


namespace _Wolframe	{

void LuaConfiguration::print( std::ostream& os ) const
{
	os << displayName() << std::endl;
	os << "   LUA script: " << script << std::endl;
	os << "        debug: " << ( debug ? "yes" : "no" ) << std::endl;
}


/// Check if the database configuration makes sense
bool LuaConfiguration::check( std::ostream& os ) const
{
	bool correct = true;

	if( script.empty( ) ) {
		os << "No Lua script given" << std::endl;
		correct = false;
	}

	return correct;
}


bool LuaConfiguration::parse( const boost::property_tree::ptree& pt, const std::string& /* nodeName */, std::ostream& os )
{
	for ( boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++ )	{
		if ( boost::algorithm::iequals( it->first, "script" ))	{
			if ( ! Configuration::getStringValue( it, displayName(), "script", script, os ))
				return false;
		} else if ( boost::algorithm::iequals( it->first, "debug" ))	{
			if ( ! Configuration::getBoolValue( it, displayName(), "debug", debug, os ))
				return false;
		} else {
			os << displayName() << ": unknown configuration option: <" << it->first << ">";
			return false;
		}
	}

	return true;
}

} // namespace _Wolframe
