//
// lua configuration functions
//

#include "handlerConfig.hpp"
#include "configHelpers.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>


namespace _Wolframe	{

void LuaConfiguration::print( std::ostream& os ) const
{
	os << displayName() << std::endl;
	os << "   LUA script: " << script << std::endl;
	if( !preload_libs.empty( ) ) {
		os << "       preload modules: ";
		for( std::list<std::string>::const_iterator it = preload_libs.begin( ); it != preload_libs.end( ); it++ ) {
			os << *it << " ";
		}
		os << std::endl;
	}
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
			if ( ! boost::filesystem::path( script ).is_absolute() )
				os << "WARNING: " << displayName() << ": script file path is not absolute: "
								   << script << std::endl;
		} else if ( boost::algorithm::iequals( it->first, "preload_lib" ))	{
			std::string preload_lib;
			if ( ! Configuration::getStringValue( it, displayName(), "preload_lib", preload_lib, os ))
				return false;
			preload_libs.push_back( preload_lib );
		} else {
			os << displayName() << ": unknown configuration option: <" << it->first << ">";
			return false;
		}
	}

	return true;
}

void LuaConfiguration::setCanonicalPathes( const std::string& refPath )
{
	if ( ! script.empty() )	{
		if ( ! boost::filesystem::path( script ).is_absolute() )
			script = resolvePath( boost::filesystem::absolute( script,
							boost::filesystem::path( refPath ).branch_path()).string());
		else
			script = resolvePath( script );
	}
}


} // namespace _Wolframe
