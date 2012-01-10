//
// application properties - implementation
//

#include "appProperties.hpp"

namespace _Wolframe	{
	static const unsigned short APP_MAJOR_VERSION = 0;
	static const unsigned short APP_MINOR_VERSION = 0;
	static const unsigned short APP_REVISION = 5;

	const char*	applicationName()			{ return "Wolframe-ECHO"; }
	const Version	applicationVersion()			{ return Version( APP_MAJOR_VERSION,
										  APP_MINOR_VERSION
										  ,APP_REVISION
									  ); }

	const char*	config::defaultMainConfig()		{ return "/etc/echo.conf"; }
	const char*	config::defaultUserConfig()		{ return "~/echo.conf"; }
	const char*	config::defaultLocalConfig()		{ return "./echo.conf"; }

	unsigned short	net::defaultTCPport()			{ return 7660; }
	unsigned short	net::defaultSSLport()			{ return 7960; }

	const char*	config::defaultServiceName()		{ return "wolframe-echo"; }
#if defined( _WIN32 )
	const char*	config::defaultServiceDisplayName()	{ return "Wolframe ECHO Daemon"; }
	const char*	config::defaultServiceDescription()	{ return "a daemon for echo wolframeing"; }
#endif // defined( _WIN32 )

} // namespace _Wolframe

