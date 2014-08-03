//
// application properties - implementation
//

#include "appProperties.hpp"
#include "wolframe.hpp"

namespace _Wolframe	{
	const char*	applicationName()			{ return "Wolframe-ECHO"; }
	const Version	applicationVersion()			{ return Version( WOLFRAME_MAJOR_VERSION,
										  WOLFRAME_MINOR_VERSION
										  ,WOLFRAME_REVISION
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

