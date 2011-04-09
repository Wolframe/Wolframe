//
// application properties - implementation
//

#include "appProperties.hpp"

namespace _Wolframe	{

	const char*	applicationName()				{ return "Wolframe-lua"; }
	unsigned short	applicationMajorVersion()			{ return 0; }
	unsigned short	applicationMinorVersion()			{ return 0; }
	unsigned short	applicationRevisionVersion()			{ return 4; }
	unsigned	applicationBuildVersion()			{ return 0; }

	const char*	config::defaultMainConfig()		{ return "/etc/lua.conf"; }
	const char*	config::defaultUserConfig()		{ return "~/lua.conf"; }
	const char*	config::defaultLocalConfig()		{ return "./lua.conf"; }

	unsigned short	config::defaultTCPport()			{ return 7660; }
	unsigned short	config::defaultSSLport()			{ return 7960; }

	const char*	config::defaultServiceName()		{ return "Wolframe-lua"; }
#if defined( _WIN32 )
	const char*	config::defaultServiceDisplayName()	{ return "Wolframe lua ECHO Daemon"; }
	const char*	config::defaultServiceDescription()	{ return "a wolframe daemon for lua echoing"; }
#endif // defined( _WIN32 )

} // namespace _Wolframe

