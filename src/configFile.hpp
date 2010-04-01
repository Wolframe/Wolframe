#ifndef _CONFIGFILE_HPP_INCLUDED
#define _CONFIGFILE_HPP_INCLUDED

#include <string>
#include <vector>

namespace _SMERP	{

	struct CfgFileConfig	{
		std::string	file;
// daemon configuration
		std::string	user;
		std::string	group;
		unsigned short	threads;
		unsigned short	maxClients;
// network configuration
		std::vector< std::pair<std::string, unsigned short> > address;
		std::vector< std::pair<std::string, unsigned short> > SSLaddress;

		unsigned	idleTimeout;
		unsigned	requestTimeout;
		unsigned	answerTimeout;
		unsigned	processTimeout;
// SSL
		std::string	SSLcertificate;
		std::string	SSLkey;
		std::string	SSLCAdirectory;
		std::string	SSLCAchainFile;
		bool		SSLverify;
// database configuration
		std::string	dbHost;
		unsigned short	dbPort;
		std::string	dbName;
		std::string	dbUser;
		std::string	dbPassword;
// logger configuration
		bool		logToStderr;
		std::string	stderrLogLevel;
		bool		logToFile;
		std::string	logFile;
		std::string	logFileLogLevel;
		bool		logToSyslog;
		std::string	syslogFacility;
		std::string	syslogLogLevel;
		bool		logToEventlog;
		std::string	eventlogSource;
		std::string	eventlogLogLevel;

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

}

#endif // _CONFIGFILE_HPP_INCLUDED
