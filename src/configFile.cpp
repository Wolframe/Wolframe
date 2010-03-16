//
// configFile.cpp
//

#include "configFile.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <vector>
#include <string>


namespace prgOpts = boost::program_options;

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


	CfgFileConfig::CfgFileConfig()
	{
	// Config file options
	options_.add_options()
// daemon configuration
			( "user,u", prgOpts::value<std::string>(), "run as <user>" )
			( "group,g", prgOpts::value<std::string>(), "run as <group>" )
			( "threads", prgOpts::value<int>(), "number of server threads" )
// network configuration
			( "listen", prgOpts::value< std::vector<std::string> >(), "addresses on which to listen (* for all)" )
			( "port", prgOpts::value<int>(), "TCP port" )
			( "SSLport", prgOpts::value<int>(), "TCP port for SSL connections" )

			( "idleTimeout", prgOpts::value<int>(), "timeout for an idle connection" )
			( "requestTimeout", prgOpts::value<int>(), "maximum time for a request" )
			( "answerTimeout", prgOpts::value<int>(), "maximum time for an answer" )
// database configuration
			( "dbHost", prgOpts::value<std::string>(), "application database host" )
			( "dbPort", prgOpts::value<std::string>(), "application database port" )
			( "dbName", prgOpts::value<std::string>(), "application database name" )
			( "dbUser", prgOpts::value<std::string>(), "application database user" )
			( "dbPassword", prgOpts::value<std::string>(), "application database password" )
// logger configuration
			( "logFile", prgOpts::value<std::string>(), "log file" )
			;
	}

	bool CfgFileConfig::parse ( const char *filename )
	{
		//

		return true;
	}

} // namespace _SMERP
