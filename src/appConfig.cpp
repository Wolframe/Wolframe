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

		if ( !cmdLine.user.empty())
			user = cmdLine.user;
		else
			user = cfgFile.user;

		if ( !cmdLine.group.empty())
			group = cmdLine.group;
		else
			group = cfgFile.group;

		pidFile = cfgFile.pidFile;
		serviceName = cfgFile.serviceName;
		serviceDisplayName = cfgFile.serviceDisplayName;
		serviceDescription = cfgFile.serviceDescription;

		threads = cfgFile.threads;
		maxClients = cfgFile.maxClients;

		address = cfgFile.address;
		SSLaddress = cfgFile.SSLaddress;

		idleTimeout = cfgFile.idleTimeout;
		requestTimeout = cfgFile.requestTimeout;
		answerTimeout = cfgFile.answerTimeout;
		processTimeout = cfgFile.processTimeout;

		dbHost = cfgFile.dbHost;
		dbPort = cfgFile.dbPort;
		dbName = cfgFile.dbName;
		dbUser = cfgFile.dbUser;
		dbPassword = cfgFile.dbPassword;

		if ( !foreground )	{
			logToStderr = cfgFile.logToStderr;
			stderrLogLevel = cfgFile.stderrLogLevel;
			logToFile = cfgFile.logToFile;
			logFile = cfgFile.logFile;
			logFileLogLevel = cfgFile.logFileLogLevel;
			logToSyslog = cfgFile.logToSyslog;
			syslogFacility = cfgFile.syslogFacility;
			syslogLogLevel = cfgFile.syslogLogLevel;
			logToEventlog = cfgFile.logToEventlog;
			eventlogLogName = cfgFile.eventlogLogName;
			eventlogSource = cfgFile.eventlogSource;
			eventlogLogLevel = cfgFile.eventlogLogLevel;
		}
		else	{
			logToStderr = true;
			stderrLogLevel = cmdLine.debugLevel;
			logToFile = false;
			logToSyslog = false;
			logToEventlog = false;
		}
	}


	void ApplicationConfiguration::print( std::ostream& os ) const
	{
		os << "Configuration file: " << configFile << std::endl;
		// from command line
		os << "Run in foreground: " << (foreground ? "yes" : "no") << std::endl;

// Unix daemon
#if !defined(_WIN32)
		os << "Run as " << (user.empty() ? "(not specified)" : user) << ":"
				<< (group.empty() ? "(not specified)" : group) << std::endl;
		os << "PID file: " << pidFile << std::endl;
		os << "Number of threads: " << threads << std::endl;
		os << "Maximum number of clients: " << maxClients << std::endl;
#endif

// Windows service
#if defined(_WIN32)
		os << "When run as service" << std::endl
			<< "  Name: " << serviceName << std::endl
			<< "  Displayed name: " << serviceDisplayName << std::endl
			<< "  Description: " << serviceDescription << std::endl;
#endif

		os << "Network" << std::endl;
		if ( address.size() > 0 )	{
			os << "  Unencrypted: " << address[0].toString() << std::endl;
			for ( unsigned i = 1; i < address.size(); i++ )
				os << "               " << address[i].toString() << std::endl;
		}
		if ( SSLaddress.size() > 0 )	{
			os << "          SSL: " << SSLaddress[0].toString() << std::endl;
			os << "                  certificate: " << (SSLaddress[0].certificate().empty() ? "(none)" : SSLaddress[0].certificate()) << std::endl;
			os << "                  key file: " << (SSLaddress[0].key().empty() ? "(none)" : SSLaddress[0].key()) << std::endl;
			os << "                  CA directory: " << (SSLaddress[0].CAdirectory().empty() ? "(none)" : SSLaddress[0].CAdirectory()) << std::endl;
			os << "                  CA chain file: " << (SSLaddress[0].CAchain().empty() ? "(none)" : SSLaddress[0].CAchain()) << std::endl;
			os << "                  verify client: " << (SSLaddress[0].verifyClientCert() ? "yes" : "no") << std::endl;
			for ( unsigned i = 1; i < SSLaddress.size(); i++ )	{
				os << "               " << SSLaddress[i].toString() << std::endl;
			}
		}

		os << "Timeouts" << std::endl;
		os << "   idle: " << idleTimeout << std::endl;
		os << "   request: " << requestTimeout << std::endl;
		os << "   answer: " << answerTimeout << std::endl;
		os << "   process: " << processTimeout << std::endl;

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
			os << "   Log to stderr, level " << stderrLogLevel << std::endl;
		else
			os << "   Log to stderr: DISABLED" << std::endl;
		if ( logToFile )	{
			os << "   Log to file: " << logFile << ", level " << logFileLogLevel << std::endl;
		}
		else
			os << "   Log to file: DISABLED" << std::endl;
		if ( logToSyslog )
			os << "   Log to syslog: facility " << syslogFacility << ", level " << syslogLogLevel << std::endl;
		else
			os << "   Log to syslog: DISABLED" << std::endl;
		if ( logToEventlog )
			os << "   Log to eventlog: name " << eventlogLogName << ", source " << eventlogSource << ", level " << eventlogLogLevel;
		else
			os << "   Log to eventlog: DISABLED" << std::endl;
	}

	/// Check if the application configuration makes sense
	///
	/// Be aware that this function does NOT test if the configuration
	/// can be used. It only tests if it MAY be valid.
	bool ApplicationConfiguration::check()
	{
		// if log to file is requested a file must be specified
		if ( logToFile )
			if ( logFile.empty())	{
				errMsg_ = "Log to file requested but no log file specified";
				return false;
			}
/*
		// if it listens to SSL a certificate file and a key file are required
		if ( SSLaddress.size() > 0 )	{
			if ( SSLcertificate.empty())	{
				errMsg_ = "SSL port defined but no SSL certificate specified";
				return false;
			}
			if ( SSLkey.empty())	{
				errMsg_ = "Server SSL certificate needs a key but no key file specified";
				return false;
			}
		}
		// verify client SSL certificate needs either certificate dir or chain file
		if ( SSLverify )	{
			if ( SSLCAdirectory.empty() && SSLCAchainFile.empty())	{
				errMsg_ = "Client SSL certificate verification requested but no CA directory or CA chain file specified";
				return false;
			}
		}
*/
		return true;
	}

} // namespace _SMERP
