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
			// daemon / service configuration
			serviceConf = new _Wolframe::Configuration::ServiceConfiguration();
			// network server
			serverConf = new _Wolframe::Network::ServerConfiguration();
			// logging
			loggerConf = new Configuration::LoggerConfiguration();

			handlerConf = new _Wolframe::HandlerConfiguration();

			// add both sections, the parse function will select the
			// appropriate action
			addConfig( "service", serviceConf );
			addConfig( "daemon", serviceConf );

			addConfig( "listen", serverConf );
			addConfig( "logging", loggerConf );
			addConfig( "lua", handlerConf->luaConfig );
		}

	} // namespace Configuration
} // namespace _Wolframe
