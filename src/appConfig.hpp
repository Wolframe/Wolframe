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

#include <string>
#include <list>

namespace _SMERP {

	struct HandlerConfiguration;

	namespace Configuration	{

		struct CmdLineConfig;		// forward declaration for the command line structure

		struct ServerConfiguration;	// forward declarations for configuration elements
		struct LoggerConfiguration;


		/// application configuration structure
		struct ApplicationConfiguration	{
			std::string			configFile;
			// from command line
			bool				foreground;

			// server configuration
			ServerConfiguration	*srvConfig;
			// logger configuration
			LoggerConfiguration	*logConfig;

			_SMERP::HandlerConfiguration	*handlerConfig;

		public:
			ApplicationConfiguration();

			bool parse( const char *filename, std::ostream& os );
			void finalize( const CmdLineConfig& cmdLine );

			bool check( std::ostream& os ) const;
			bool test( std::ostream& os ) const;
			void print( std::ostream& os ) const;

			static const char* chooseFile( const char *globalFile,
						       const char *userFile,
						       const char *localFile );
		};
	} // namespace Configuration
} // namespace _SMERP

#endif // _APP_CONFIG_HPP_INCLUDED
