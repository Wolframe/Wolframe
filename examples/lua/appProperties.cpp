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

	const char*	Configuration::defaultMainConfig()		{ return "/etc/lua.conf"; }
	const char*	Configuration::defaultUserConfig()		{ return "~/lua.conf"; }
	const char*	Configuration::defaultLocalConfig()		{ return "./lua.conf"; }

	unsigned short	Configuration::defaultTCPport()			{ return 7660; }
	unsigned short	Configuration::defaultSSLport()			{ return 7960; }

	const char*	Configuration::defaultServiceName()		{ return "Wolframe-lua"; }
#if defined( _WIN32 )
	const char*	Configuration::defaultServiceDisplayName()	{ return "Wolframe lua ECHO Daemon"; }
	const char*	Configuration::defaultServiceDescription()	{ return "a wolframe daemon for lua echoing"; }
#endif // defined( _WIN32 )

} // namespace _Wolframe

