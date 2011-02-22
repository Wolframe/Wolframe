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

	struct ApplicationConfiguration	{
		std::string		configFile;
// from command line
		bool			foreground;

// server configuration
		Configuration::ServerConfiguration	*srvConfig;
// database configuration
		Configuration::DatabaseConfiguration	*dbConfig;
// logger configuration
		Configuration::LoggerConfiguration	*logConfig;

	private:
		std::string		errMsg_;

	public:
		ApplicationConfiguration();

		bool parse( const char *filename, std::ostream& os );
		void finalize( const CmdLineConfig& cmdLine );

		std::string errMsg() const;
		bool check( std::ostream& os ) const;
		bool test( std::ostream& os ) const;
		void print( std::ostream& os ) const;

		static const char* chooseFile( const char *globalFile,
						const char *userFile,
						const char *localFile );

	};

} // namespace _SMERP

#endif // _APP_CONFIG_HPP_INCLUDED
