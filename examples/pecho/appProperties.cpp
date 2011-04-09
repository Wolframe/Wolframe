//
// application properties - implementation
//

#include "appProperties.hpp"

namespace _Wolframe	{

	const char*	applicationName()				{ return "Wolframe-PECHO"; }
	unsigned short	applicationMajorVersion()			{ return 0; }
	unsigned short	applicationMinorVersion()			{ return 0; }
	unsigned short	applicationRevisionVersion()			{ return 4; }
	unsigned	applicationBuildVersion()			{ return 0; }

	const char*	config::defaultMainConfig()		{ return "/etc/wolframe-pecho.conf"; }
	const char*	config::defaultUserConfig()		{ return "~/wolframe-pecho.conf"; }
	const char*	config::defaultLocalConfig()		{ return "./wolframe-pecho.conf"; }

	unsigned short	config::defaultTCPport()			{ return 7660; }
	unsigned short	config::defaultSSLport()			{ return 7960; }

	const char*	config::defaultServiceName()		{ return "wolframe-pecho"; }
#if defined( _WIN32 )
	const char*	config::defaultServiceDisplayName()	{ return "Wolframe ECHO Daemon"; }
	const char*	config::defaultServiceDescription()	{ return "a Wolframe daemon for echo"; }
#endif // defined( _WIN32 )

} // namespace _Wolframe

