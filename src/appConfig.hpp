//
// appConfig.hpp
//

#ifndef _APP_CONFIG_HPP_INCLUDED
#define _APP_CONFIG_HPP_INCLUDED

#include "logLevel.hpp"

#if !defined( _WIN32 )
#include "logSyslogFacility.hpp"
#endif	// !defined( _WIN32 )

#include "serverEndpoint.hpp"

#include "configStandard.hpp"

#include <string>
#include <list>

namespace _SMERP {


/// application configuration structure
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
		unsigned		maxConnections;

// network configuration
		std::list<Network::ServerTCPendpoint> address;
		std::list<Network::ServerSSLendpoint> SSLaddress;

// server configuration
		Configuration::ServerConfiguration	*srvConfig;
// database configuration
		Configuration::DatabaseConfiguration	*dbConfig;
// logger configuration
		Configuration::LoggerConfiguration	*logConfig;

	private:
		std::string		errMsg_;

	public:
		ApplicationConfiguration( const CmdLineConfig& cmdLine, const CfgFileConfig& cfgFile );
		std::string errMsg() const	{ return errMsg_; }
		bool check();
		bool test();
		void print( std::ostream& os ) const;
	};

} // namespace _SMERP

#endif // _APP_CONFIG_HPP_INCLUDED
