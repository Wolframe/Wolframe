//
// appConfigDefinition.cpp
//

#include "appConfig.hpp"
#include "standardConfigs.hpp"
#include "handlerConfig.hpp"


namespace _Wolframe {
	namespace Configuration {

ApplicationConfiguration::ApplicationConfiguration()
{
	// server
	srvConfig = new Configuration::ServerConfiguration( "server", "Server" );

	// logging
	logConfig = new Configuration::LoggerConfiguration( "logging", "Logging" );

	handlerConfig = new _Wolframe::HandlerConfiguration();

	section["server"] = 0, conf.push_back( srvConfig );
	section["logging"] = 1, conf.push_back( logConfig );
	section["database"] = 2, conf.push_back( handlerConfig->database );
}

	} // namespace Configuration
} // namespace _Wolframe
