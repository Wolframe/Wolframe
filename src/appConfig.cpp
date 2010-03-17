//
// appConfig.cpp
//

#include "appConfig.hpp"
#include "commandLine.hpp"
#include "configFile.hpp"

#include <string>
#include <ostream>

namespace _SMERP {

	ApplicationConfiguration::ApplicationConfiguration( const CmdLineConfig& cmdLine, const CfgFileConfig& cfgFile )
	{
		configFile = cfgFile.file;

		foreground = cmdLine.foreground;
		debug = cmdLine.debugLevel;

		if ( !cmdLine.user.empty())
			user = cfgFile.user;
		else
			user = cmdLine.user;

		if ( !cmdLine.group.empty())
			group = cfgFile.group;
		else
			group = cmdLine.group;

		address = cfgFile.address;
		port = cfgFile.port;
		SSLport = cfgFile.SSLport;
		dbHost = cfgFile.dbHost;
		dbPort = cfgFile.dbPort;
		dbName = cfgFile.dbName;
		dbUser = cfgFile.dbUser;
		dbPassword = cfgFile.dbPassword;

		logFile = cfgFile.logFile;
	}


	void ApplicationConfiguration::print( std::ostream& os ) const
	{
		os << "Configuration file: " << configFile << std::endl;
		// from command line
		os << "Run in foreground: " << foreground << std::endl;
		os << "   Debug level: " << debug;
		if ( !foreground )
			os << " (inactive)" << std::endl;
		else
			os << std::endl;

		os << "Run as " << (user.empty() ? "(not specified)" : user) << ":"
				<< (group.empty() ? "(not specified)" : group) << std::endl;
		os << "Number of threads: " << threads << std::endl;
		os << "Maximum number of clients: " << maxClients << std::endl;

		os << "Network" << std::endl;
		os << "  Listen on:";
		for ( unsigned i = 0; i < address.size(); i++ )
			os << " " << address[i];
		os << std::endl;
		if ( port )
			os << "   Unencrypted connections on port " << port << std::endl;
		if ( SSLport )
			os << "   SSL connections on port " << SSLport << std::endl;

		os << "Timeouts" << std::endl;
		os << "   idle: " << idleTimeout << std::endl;
		os << "   request: " << requestTimeout << std::endl;
		os << "   answer: " << answerTimeout << std::endl;

		os << "Database" << std::endl;
		os << "   DB host: " << dbHost << ":" << dbPort << std::endl;
		os << "   DB name: " << (dbName.empty() ? "(not specified)" : dbName) << std::endl;
		os << "   DB user / password: " << (dbUser.empty() ? "(not specified)" : dbUser) << " / "
						<< (dbPassword.empty() ? "(not specified)" : dbPassword) << std::endl;

		os << "Logging" << std::endl;
		os << "   Log file: " << logFile << std::endl;
	}

} // namespace _SMERP
