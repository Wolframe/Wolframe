//
// appConfigDefinition.cpp
//

#include "appConfig.hpp"
#include "standardConfigs.hpp"
#include "handlerConfig.hpp"

#include <iostream>

namespace _Wolframe {
	namespace Configuration {

		ApplicationConfiguration::ApplicationConfiguration()
		{
			// server
			srvConfig = new Configuration::ServerConfiguration( "server", "Server" );

			// logging
			logConfig = new Configuration::LoggerConfiguration( "logging", "Logging" );

			handlerConfig = new _Wolframe::HandlerConfiguration();

			addConfig( srvConfig );
			addConfig( logConfig );
			addConfig( handlerConfig->database );
		}

	} // namespace Configuration
} // namespace _Wolframe
