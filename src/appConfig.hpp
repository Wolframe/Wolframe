//
// appConfig.hpp
//

#ifndef _APP_CONFIG_HPP_INCLUDED
#define _APP_CONFIG_HPP_INCLUDED

#include <string>
#include <vector>


namespace _SMERP {

// application configuration structure

	struct CmdLineConfig;		// for configuration from the command line
	struct CfgFileConfig;		// for configuration from the config file

	struct ApplicationConfiguration	{
		std::string	configFile;
// from command line
		bool		foreground;
		unsigned short	debug;
// daemon configuration
		std::string	user;
		std::string	group;
		unsigned	threads;
		unsigned	maxClients;

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
		std::string	logFile;
	public:
		ApplicationConfiguration( const CmdLineConfig& cmdLine, const CfgFileConfig& cfgFile );
		void print( std::ostream& os ) const;
	};
} // namespace _SMERP

#endif // _APP_CONFIG_HPP_INCLUDED
