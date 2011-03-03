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
			srvConfig = new Configuration::ServerConfiguration();

			// logging
			logConfig = new Configuration::LoggerConfiguration();

			handlerConfig = new _Wolframe::HandlerConfiguration();

			addConfig( "server", srvConfig );
			addConfig( "logging", logConfig );
			addConfig( "lua", handlerConfig->luaConfig );
		}

	} // namespace Configuration
} // namespace _Wolframe
