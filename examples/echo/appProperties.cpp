//
// application properties - implementation
//

#include "appProperties.hpp"

namespace _Wolframe	{

	const char*	applicationName()				{ return "Wolframe-ECHO"; }
	unsigned short	applicationMajorVersion()			{ return 0; }
	unsigned short	applicationMinorVersion()			{ return 0; }
	unsigned short	applicationRevisionVersion()			{ return 4; }
	unsigned	applicationBuildVersion()			{ return 0; }

	const char*	config::defaultMainConfig()		{ return "/etc/echo.conf"; }
	const char*	config::defaultUserConfig()		{ return "~/echo.conf"; }
	const char*	config::defaultLocalConfig()		{ return "./echo.conf"; }

	unsigned short	config::defaultTCPport()			{ return 7660; }
	unsigned short	config::defaultSSLport()			{ return 7960; }

	const char*	config::defaultServiceName()		{ return "wolframe-echo"; }
#if defined( _WIN32 )
	const char*	config::defaultServiceDisplayName()	{ return "Wolframe ECHO Daemon"; }
	const char*	config::defaultServiceDescription()	{ return "a daemon for echo wolframeing"; }
#endif // defined( _WIN32 )

} // namespace _Wolframe

