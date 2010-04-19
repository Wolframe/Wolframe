#ifndef _CONFIGFILE_HPP_INCLUDED
#define _CONFIGFILE_HPP_INCLUDED

#include "logLevel.hpp"
#include "logSyslogFacility.hpp"

#include <string>
#include <vector>

namespace _SMERP	{

	struct CfgFileConfig	{
		std::string	file;
// daemon configuration
		std::string	user;
		std::string	group;
		std::string	pidFile;

// service configuration
		std::string	serviceName;
		std::string	serviceDisplayName;
		std::string	serviceDescription;

// server configuration
		unsigned short	threads;
		unsigned short	maxClients;
// network configuration
		std::vector<struct localEndpoint> address;
		std::vector<struct localSSLendpoint> SSLaddress;

		unsigned	idleTimeout;
		unsigned	requestTimeout;
		unsigned	answerTimeout;
		unsigned	processTimeout;

// database configuration
		std::string	dbHost;
		unsigned short	dbPort;
		std::string	dbName;
		std::string	dbUser;
		std::string	dbPassword;
// logger configuration
		bool		logToStderr;
		LogLevel::Level	stderrLogLevel;
		bool		logToFile;
		std::string	logFile;
		LogLevel::Level	logFileLogLevel;
		bool		logToSyslog;
		SyslogFacility::Facility syslogFacility;
		LogLevel::Level	syslogLogLevel;
		bool		logToEventlog;
		std::string	eventlogLogName;
		std::string	eventlogSource;
		LogLevel::Level	eventlogLogLevel;

	private:
		std::string	errMsg_;

	public:
		CfgFileConfig();
		static const char* chooseFile( const char *globalFile,
						const char *userFile,
						const char *localFile );

		bool parse( const char *filename );
		std::string errMsg( void )	{ return errMsg_; };
	};

} // namespace _SMERP

#endif // _CONFIGFILE_HPP_INCLUDED
