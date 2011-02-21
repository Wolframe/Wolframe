//
// configFile.cpp
//

#include "configFile.hpp"
#include "serverEndpoint.hpp"

#include "miscUtils.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <vector>

#include <sstream>


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
