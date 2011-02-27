//
// application properties - implementation
//

#include "appProperties.hpp"

namespace _SMERP	{

	const char*	applicationName()				{ return "Wolframe"; }
	unsigned short	applicationMajorVersion()			{ return 0; }
	unsigned short	applicationMinorVersion()			{ return 0; }
	unsigned short	applicationRevisionVersion()			{ return 4; }
	unsigned	applicationBuildVersion()			{ return 0; }

	const char*	Configuration::defaultMainConfig()		{ return "/etc/wolframe.conf"; }
	const char*	Configuration::defaultUserConfig()		{ return "~/wolframe.conf"; }
	const char*	Configuration::defaultLocalConfig()		{ return "./wolframe.conf"; }

	unsigned short	Configuration::defaultTCPport()			{ return 7660; }
	unsigned short	Configuration::defaultSSLport()			{ return 7960; }

	const char*	Configuration::defaultServiceName()		{ return "wolframe"; }
#if defined( _WIN32 )
	const char*	Configuration::defaultServiceDisplayName()	{ return "Wolframe Daemon"; }
	const char*	Configuration::defaultServiceDescription()	{ return "a daemon for wolframeing"; }
#endif // defined( _WIN32 )

} // namespace _SMERP

