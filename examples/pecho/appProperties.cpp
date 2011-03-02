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

	const char*	Configuration::defaultMainConfig()		{ return "/etc/wolframe-pecho.conf"; }
	const char*	Configuration::defaultUserConfig()		{ return "~/wolframe-pecho.conf"; }
	const char*	Configuration::defaultLocalConfig()		{ return "./wolframe-pecho.conf"; }

	unsigned short	Configuration::defaultTCPport()			{ return 7660; }
	unsigned short	Configuration::defaultSSLport()			{ return 7960; }

	const char*	Configuration::defaultServiceName()		{ return "wolframe-pecho"; }
#if defined( _WIN32 )
	const char*	Configuration::defaultServiceDisplayName()	{ return "Wolframe ECHO Daemon"; }
	const char*	Configuration::defaultServiceDescription()	{ return "a Wolframe daemon for echo"; }
#endif // defined( _WIN32 )

} // namespace _Wolframe

