//
// application properties - implementation
//

#include "appProperties.hpp"

namespace _SMERP	{

	const char*	applicationName()				{ return "Smerp"; }
	unsigned short	applicationMajorVersion()			{ return 0; }
	unsigned short	applicationMinorVersion()			{ return 0; }
	unsigned short	applicationRevisionVersion()			{ return 4; }
	unsigned	applicationBuildVersion()			{ return 0; }

	const char*	Configuration::defaultMainConfig()		{ return "/etc/smerp.conf"; }
	const char*	Configuration::defaultUserConfig()		{ return "~/smerp.conf"; }
	const char*	Configuration::defaultLocalConfig()		{ return "./smerp.conf"; }

	unsigned short	Configuration::defaultTCPport()			{ return 7660; }
	unsigned short	Configuration::defaultSSLport()			{ return 7960; }

	const char*	Configuration::defaultServiceName()		{ return "smerp"; }
#if defined( _WIN32 )
	const char*	Configuration::defaultServiceDisplayName()	{ return "Smerp Daemon"; }
	const char*	Configuration::defaultServiceDescription()	{ return "a daemon for smerping"; }
#endif // defined( _WIN32 )

} // namespace _SMERP

