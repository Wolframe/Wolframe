//
// appConfig.hpp
//

#ifndef _APP_CONFIG_HPP_INCLUDED
#define _APP_CONFIG_HPP_INCLUDED

#include "logLevel.hpp"

#include <string>
#include <vector>

namespace _SMERP {

// application configuration structure

	struct CmdLineConfig;		// forward declaration for configuration from the command line
	struct CfgFileConfig;		// forward declaration for configuration from the config file


	/// structure for local endpoint configuration
	struct localEndpoint	{
		std::string	host;
		unsigned short	port;
	};

	/// structure for local SSL endpoint configuration
	struct localSSLendpoint	{
		std::string	host;
		unsigned short	port;
		std::string	certFile;
		std::string	keyFile;
		std::string	CAdirectory;
		std::string	CAchainFile;
		bool		verify;
	};


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
		std::vector< std::pair<std::string, unsigned short> > address;
		std::vector< std::pair<std::string, unsigned short> > SSLaddress;

		unsigned		idleTimeout;
		unsigned		requestTimeout;
		unsigned		answerTimeout;
		unsigned		processTimeout;
// SSL
		std::string	SSLcertificate;
		std::string	SSLkey;
		std::string	SSLCAdirectory;
		std::string	SSLCAchainFile;
		bool		SSLverify;

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
		std::string	syslogFacility;
		LogLevel::Level	syslogLogLevel;
		bool		logToEventlog;
		std::string	eventlogLogName;
		std::string	eventlogSource;
		LogLevel::Level	eventlogLogLevel;

	private:
		std::string		errMsg_;

	public:
		ApplicationConfiguration( const CmdLineConfig& cmdLine, const CfgFileConfig& cfgFile );
		std::string errMsg() const	{ return errMsg_; };
		bool check();
		void print( std::ostream& os ) const;
	};
} // namespace _SMERP

#endif // _APP_CONFIG_HPP_INCLUDED
