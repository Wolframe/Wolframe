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

	const char*	config::defaultMainConfig()		{ return "/etc/mtproc.conf"; }
	const char*	config::defaultUserConfig()		{ return "~/mtproc.conf"; }
	const char*	config::defaultLocalConfig()		{ return "./mtproc.conf"; }

	unsigned short	config::defaultTCPport()			{ return 7660; }
	unsigned short	config::defaultSSLport()			{ return 7960; }

	const char*	config::defaultServiceName()		{ return "mtproc"; }
#if defined( _WIN32 )
	const char*	config::defaultServiceDisplayName()	{ return "Wolframe mtproc Daemon"; }
	const char*	config::defaultServiceDescription()	{ return "a daemon for mtprocing"; }
#endif // defined( _WIN32 )

} // namespace _Wolframe

