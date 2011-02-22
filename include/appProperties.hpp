//
// application properties
//

#ifndef _APP_PROPERTIES_HPP_INCLUDED
#define _APP_PROPERTIES_HPP_INCLUDED

#include "version.hpp"

namespace _SMERP	{

	const char*		applicationName();
	unsigned short		applicationMajorVersion();
	unsigned short		applicationMinorVersion();
	unsigned short		applicationRevisionVersion();
	unsigned		applicationBuildVersion();

	namespace Configuration	{

		const char*	defaultMainConfig();
		const char*	defaultUserConfig();
		const char*	defaultLocalConfig();

		unsigned short	defaultTCPport();
		unsigned short	defaultSSLport();

		const char*	defaultServiceName();
#if defined( _WIN32 )
		const char*	defaultServiceDisplayName();
		const char*	defaultServiceDescription();
#endif // defined( _WIN32 )

	} // namespace Configuration
} // namespace _SMERP

#endif // _APP_PROPERTIES_HPP_INCLUDED
