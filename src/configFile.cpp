//
// configFile.cpp
//

#include "configFile.hpp"
#include "serverEndpoint.hpp"

#include "miscUtils.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <vector>
#include <string>

#include <sstream>


static const unsigned short	DEFAULT_PORT = 7660;
static const unsigned short	SSL_DEFAULT_PORT = 7960;

static const char*		DEFAULT_SERVICE_NAME = "smerp";
static const char*		DEFAULT_SERVICE_DISPLAY_NAME = "Smerp Daemon";
static const char*		DEFAULT_SERVICE_DESCRIPTION = "a daemon for smerping";


static boost::logic::tribool getBoolValue( boost::property_tree::ptree& pt, const std::string& label, std::string& val )
{
	std::string s = pt.get<std::string>( label, std::string() );
	val = s;
	boost::to_upper( s );
	boost::trim( s );
	if ( s == "NO" || s == "FALSE" || s == "0" || s == "OFF" )
		return false;
	if ( s == "YES" || s == "TRUE" || s == "1" || s == "ON" )
		return true;
	return boost::logic::indeterminate;
}


namespace _SMERP {

	const char* CfgFileConfig::chooseFile( const char *globalFile, const char *userFile, const char *localFile )
	{
		if ( globalFile != NULL )
			if ( boost::filesystem::exists( globalFile ))
				return globalFile;
		if ( userFile != NULL )
			if ( boost::filesystem::exists( userFile ))
				return userFile;
		if ( localFile != NULL )
			if ( boost::filesystem::exists( localFile ))
				return localFile;
		return NULL;
	}


	bool CfgFileConfig::parse ( const char *filename )
	{
		file = resolvePath( boost::filesystem::absolute( filename ).string() );
		if ( !boost::filesystem::exists( file ))	{
			errMsg_ = "Configuration file ";
			errMsg_ += file;
			errMsg_ += " does not exist.";
			return false;
		}

		// Create an empty property tree object
		boost::property_tree::ptree	pt;
		try	{

		read_info( filename, pt );

		std::stringstream	errStr;
// server
		srvConfig = new Configuration::ServerConfiguration( "server", "Server" );
		if ( ! srvConfig->parse( pt.get_child( "server" ), errStr ))	{
			errMsg_ = errStr.str();
			return false;
		}
// database
		dbConfig = new Configuration::DatabaseConfiguration( "database", "Database Server" );
		if ( ! dbConfig->parse( pt.get_child( "database" ), errStr ))	{
			errMsg_ = errStr.str();
			return false;
		}

// logging
		logConfig = new Configuration::LoggerConfiguration( "logging", "Logging" );
		if ( ! logConfig->parse( pt.get_child( "logging" ), errStr ))	{
			errMsg_ = errStr.str();
			return false;
		}
	}
	catch( std::exception& e)	{
		errMsg_ = e.what();
		return false;
	}

		return true;
	}

} // namespace _SMERP
