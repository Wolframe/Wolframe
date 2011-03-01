//
// application properties - implementation
//

#include "appProperties.hpp"

namespace _Wolframe	{

	const char*	applicationName()				{ return "mtproc"; }
	unsigned short	applicationMajorVersion()			{ return 0; }
	unsigned short	applicationMinorVersion()			{ return 0; }
	unsigned short	applicationRevisionVersion()			{ return 4; }
	unsigned	applicationBuildVersion()			{ return 0; }

	const char*	Configuration::defaultMainConfig()		{ return "/etc/mtproc.conf"; }
	const char*	Configuration::defaultUserConfig()		{ return "~/mtproc.conf"; }
	const char*	Configuration::defaultLocalConfig()		{ return "./mtproc.conf"; }

	unsigned short	Configuration::defaultTCPport()			{ return 7660; }
	unsigned short	Configuration::defaultSSLport()			{ return 7960; }

	const char*	Configuration::defaultServiceName()		{ return "mtproc"; }
#if defined( _WIN32 )
	const char*	Configuration::defaultServiceDisplayName()	{ return "Wolframe mtproc Daemon"; }
	const char*	Configuration::defaultServiceDescription()	{ return "a daemon for mtprocing"; }
#endif // defined( _WIN32 )

} // namespace _Wolframe

