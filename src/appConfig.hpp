//
// appConfig.hpp
//

#ifndef _APP_CONFIG_HPP_INCLUDED
#define _APP_CONFIG_HPP_INCLUDED

#include "logLevel.hpp"
#include "logSyslogFacility.hpp"
#include "serverEndpoint.hpp"

#include <string>
#include <vector>

namespace _SMERP {

// application configuration structure

	struct CmdLineConfig;		// forward declaration for configuration from the command line
	struct CfgFileConfig;		// forward declaration for configuration from the config file


	struct ApplicationConfiguration	{
		std::string		configFile;
// from command line
		bool			foreground;
// daemon configuration
		std::string		user;
		std::string		group;
		std::string		pidFile;
// service configuration
		std::string		serviceName;
		std::string		serviceDisplayName;
		std::string		serviceDescription;

// server configuration
		unsigned		threads;
		unsigned		maxClients;

// network configuration
		std::vector<ServerTCPendpoint> address;
		std::vector<ServerSSLendpoint> SSLaddress;

		unsigned		idleTimeout;
		unsigned		requestTimeout;
		unsigned		answerTimeout;
		unsigned		processTimeout;

// database configuration
		std::string		dbHost;
		unsigned short		dbPort;
		std::string		dbName;
		std::string		dbUser;
		std::string		dbPassword;
// logger configuration
		bool			logToStderr;
		LogLevel::Level	stderrLogLevel;
		bool		logToFile;
		std::string	logFile;
		LogLevel::Level	logFileLogLevel;
		bool		logToSyslog;
		SyslogFacility::Facility syslogFacility;
		LogLevel::Level	syslogLogLevel;
		std::string	syslogIdent;
		bool		logToEventlog;
		std::string	eventlogLogName;
		std::string	eventlogSource;
		LogLevel::Level	eventlogLogLevel;

	private:
		std::string		errMsg_;

	public:
		ApplicationConfiguration( const CmdLineConfig& cmdLine, const CfgFileConfig& cfgFile );
		std::string errMsg() const	{ return errMsg_; }
		bool check();
		void print( std::ostream& os ) const;
	};
} // namespace _SMERP

#endif // _APP_CONFIG_HPP_INCLUDED
