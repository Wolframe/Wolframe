//
// appConfig.cpp
//

#include "appConfig.hpp"
#include "commandLine.hpp"
#include "configFile.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>


namespace _SMERP {

	ApplicationConfiguration::ApplicationConfiguration( const CmdLineConfig& cmdLine, const CfgFileConfig& cfgFile )
	{
		configFile = cfgFile.file;

#if defined(_WIN32)
		// on Windows the user should either use -f or start in the console
		// (assuming an implicit -f) or the service option should contain
		// --service in the startup parameters of the service impling non-foreground
		if( cmdLine.command == _SMERP::CmdLineConfig::RUN_SERVICE )
			foreground = false;
		else
			foreground = true;
#else
		foreground = cmdLine.foreground;
#endif
		srvConfig = cfgFile.srvConfig;
		dbConfig = cfgFile.dbConfig;
		logConfig = cfgFile.logConfig;

		if ( foreground )
			logConfig->foreground( cmdLine.debugLevel, cmdLine.useLogConfig );
		srvConfig->override( cmdLine.user, cmdLine.group );
	}


	void ApplicationConfiguration::print( std::ostream& os ) const
	{

		os << "Configuration file: " << configFile << std::endl;
// Unix daemon
#if !defined(_WIN32)
		os << "Run in foreground: " << (foreground ? "yes" : "no") << std::endl;
#endif
		srvConfig->print( os );
		dbConfig->print( os );
		logConfig->print( os );
	}

	/// Check if the application configuration makes sense
	bool ApplicationConfiguration::check()
	{
		std::stringstream	errStr;

		// check logging
		if ( ! logConfig->check( errStr ))	{
			errMsg_ = errStr.str();
			return false;
		}

		return true;
	}

} // namespace _SMERP
