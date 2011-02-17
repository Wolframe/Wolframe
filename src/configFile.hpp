#ifndef _CONFIGFILE_HPP_INCLUDED
#define _CONFIGFILE_HPP_INCLUDED

#include "logLevel.hpp"
#include "logSyslogFacility.hpp"
#include "serverEndpoint.hpp"

#include "appConfig.hpp"

#include <string>
#include <list>

namespace _SMERP	{

	struct CfgFileConfig	{
		std::string		file;
// daemon configuration
		std::string		user;
		std::string		group;
		std::string		pidFile;

// service configuration
		std::string		serviceName;
		std::string		serviceDisplayName;
		std::string		serviceDescription;

// server configuration
		unsigned short		threads;
		unsigned short		maxConnections;
// network configuration
		std::list<Network::ServerTCPendpoint> address;
		std::list<Network::ServerSSLendpoint> SSLaddress;

// database configuration
		DatabaseConfiguration	*dbConfig;
// logger configuration
		LoggerConfiguration	*logConfig;

	private:
		std::string		errMsg_;

	public:
		static const char* chooseFile( const char *globalFile,
						const char *userFile,
						const char *localFile );

		bool parse( const char *filename );
		std::string errMsg( void )		{ return errMsg_; }
	};

} // namespace _SMERP

#endif // _CONFIGFILE_HPP_INCLUDED
