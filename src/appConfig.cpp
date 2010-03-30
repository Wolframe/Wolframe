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
			user = cmdLine.user;
		else
			user = cfgFile.user;

		if ( !cmdLine.group.empty())
			group = cmdLine.group;
		else
			group = cfgFile.group;

		threads = cfgFile.threads;
		maxClients = cfgFile.maxClients;

		address = cfgFile.address;
		SSLaddress = cfgFile.SSLaddress;

		SSLcertificate = cfgFile.SSLcertificate;
		SSLkey = cfgFile.SSLkey;
		SSLCAdirectory = cfgFile.SSLCAdirectory;
		SSLCAchainFile = cfgFile.SSLCAchainFile;
		SSLverify = cfgFile.SSLverify;

		idleTimeout = cfgFile.idleTimeout;
		requestTimeout = cfgFile.requestTimeout;
		answerTimeout = cfgFile.answerTimeout;
		processTimeout = cfgFile.processTimeout;

		dbHost = cfgFile.dbHost;
		dbPort = cfgFile.dbPort;
		dbName = cfgFile.dbName;
		dbUser = cfgFile.dbUser;
		dbPassword = cfgFile.dbPassword;

		logToStderr = cfgFile.logToStderr;
		stderrLogLevel = cfgFile.stderrLogLevel;
		logToFile = cfgFile.logToFile;
		logFile = cfgFile.logFile;
		logFileLogLevel = cfgFile.logFileLogLevel;
		logToSyslog = cfgFile.logToSyslog;
		syslogFacility = cfgFile.syslogFacility;
		syslogLogLevel = cfgFile.syslogLogLevel;
	}


	void ApplicationConfiguration::print( std::ostream& os ) const
	{
		os << "Configuration file: " << configFile << std::endl;
		// from command line
		os << "Run in foreground: " << (foreground ? "yes" : "no") << std::endl;
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
		if ( address.size() > 0 )	{
			os << "  Unencrypted: " << address[0].first << ":" << address[0].second << std::endl;
			for ( unsigned i = 1; i < address.size(); i++ )
				os << "               " << address[i].first << ":" << address[i].second << std::endl;
		}
		if ( SSLaddress.size() > 0 )	{
			os << "          SSL: " << SSLaddress[0].first << ":" << SSLaddress[0].second << std::endl;
			for ( unsigned i = 1; i < SSLaddress.size(); i++ )
				os << "               " << SSLaddress[i].first << ":" << SSLaddress[i].second << std::endl;
		}

		os << "Timeouts" << std::endl;
		os << "   idle: " << idleTimeout << std::endl;
		os << "   request: " << requestTimeout << std::endl;
		os << "   answer: " << answerTimeout << std::endl;
		os << "   process: " << processTimeout << std::endl;

		os << "SSL" << std::endl;
		os << "   certificate: " << (SSLcertificate.empty() ? "(none)" : SSLcertificate) << std::endl;
		os << "   key file: " << (SSLkey.empty() ? "(none)" : SSLkey) << std::endl;
		os << "   CA directory: " << (SSLCAdirectory.empty() ? "(none)" : SSLCAdirectory) << std::endl;
		os << "   CA chain file: " << (SSLCAchainFile.empty() ? "(none)" : SSLCAchainFile) << std::endl;
		os << "   verify client: " << (SSLverify ? "yes" : "no") << std::endl;

		os << "Database" << std::endl;
		if ( dbHost.empty())
			os << "   DB host: local unix domain socket" << std::endl;
		else
			os << "   DB host: " << dbHost << ":" << dbPort << std::endl;
		os << "   DB name: " << (dbName.empty() ? "(not specified - server user default)" : dbName) << std::endl;
		os << "   DB user / password: " << (dbUser.empty() ? "(not specified - same as server user)" : dbUser) << " / "
						<< (dbPassword.empty() ? "(not specified - no password used)" : dbPassword) << std::endl;

		os << "Logging" << std::endl;
		if ( logToStderr )
			os << "   Log to stderr log level: " << stderrLogLevel << std::endl;
		else
			os << "   Log to stderr: DISABLED" << std::endl;
		if ( logToFile )	{
			os << "   Log to file: " << logFile << std::endl;
			os << "   Log to file log level: " << logFileLogLevel << std::endl;
		}
		else
			os << "   Log to file: DISABLED" << std::endl;
		if ( logToSyslog )
			os << "   Log to syslog: facility " << syslogFacility << ", level " << syslogLogLevel << std::endl;
		else
			os << "   Log to syslog: DISABLED" << std::endl;
	}


	bool ApplicationConfiguration::check()
	{
		return true;
	}

} // namespace _SMERP
