//
// application defaults
//

#ifndef _APP_DEFAULTS_HPP_INCLUDED
#define _APP_DEFAULTS_HPP_INCLUDED


namespace _SMERP	{
	namespace Configuration	{

		static const unsigned short	DEFAULT_TCP_PORT = 7660;
		static const unsigned short	DEFAULT_SSL_PORT = 7960;

#if defined( _WIN32 )
		static const char*		DEFAULT_SERVICE_NAME = "smerp";
		static const char*		DEFAULT_SERVICE_DISPLAY_NAME = "Smerp Daemon";
		static const char*		DEFAULT_SERVICE_DESCRIPTION = "a daemon for smerping";
#endif // defined( _WIN32 )

	} // namespace Configuration
} // namespace _SMERP

#endif // _APP_DEFAULTS_HPP_INCLUDED
