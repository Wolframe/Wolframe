//
// appConfig.hpp
//

#ifndef _APP_CONFIG_HPP_INCLUDED
#define _APP_CONFIG_HPP_INCLUDED

#include "configurationBase.hpp"

#include <string>
#include <map>
#include <vector>

namespace _Wolframe {

	// forward declarations for configuration elements
	struct HandlerConfiguration;
	namespace Network	{
		struct ServerConfiguration;
	} // namespace Network

	namespace Configuration	{

		struct ServiceConfiguration;
		struct LoggerConfiguration;
		struct CmdLineConfig;		// forward declaration for the command line structure

		/// application configuration structure
		struct ApplicationConfiguration	{
			std::string				configFile;
			// from command line
			bool					foreground;

			// daemon / service configuration
			ServiceConfiguration			*serviceConf;
			// network server configuration
			_Wolframe::Network::ServerConfiguration	*serverConf;
			// logger configuration
			LoggerConfiguration			*loggerConf;

			_Wolframe::HandlerConfiguration		*handlerConf;

		public:
			ApplicationConfiguration();
			~ApplicationConfiguration();

			bool parse( const char *filename, std::ostream& os );
			void finalize( const CmdLineConfig& cmdLine );

			bool check( std::ostream& os ) const;
			bool test( std::ostream& os ) const;
			void print( std::ostream& os ) const;

			static const char* chooseFile( const char *globalFile,
						       const char *userFile,
						       const char *localFile );
		private:
			std::vector< ConfigurationBase* >	conf_;
			std::map< std::string, std::size_t >	section_;

			bool addConfig( const std::string& nodeName, ConfigurationBase *config );
		};
	} // namespace Configuration
} // namespace _Wolframe

#endif // _APP_CONFIG_HPP_INCLUDED
