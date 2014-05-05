//
// echod.cpp
//
#include "appProperties.hpp"
#include "version.hpp"
#include "wolframe.hpp"

// Super main function :)

class EchoAppProperties
	:public _Wolframe::AppProperties
{
public:
	virtual const char* applicationName() const		{ return "Wolframe-ECHO"; }
	virtual const _Wolframe::Version applicationVersion() const	{ return _Wolframe::Version(
										  WOLFRAME_MAJOR_VERSION,
										  WOLFRAME_MINOR_VERSION,
										  WOLFRAME_REVISION
										  ); }

	virtual const char* defaultMainConfig() const		{ return "/etc/echo.conf"; }
	virtual const char* defaultUserConfig() const		{ return "~/echo.conf"; }
	virtual const char* defaultLocalConfig() const		{ return "./echo.conf"; }

	virtual unsigned short defaultTCPport() const		{ return 7660; }
	virtual unsigned short defaultSSLport() const		{ return 7960; }

	virtual const char* defaultServiceName() const		{ return "wolframe-echo"; }
#if defined( _WIN32 )
	const char* defaultServiceDisplayName() const		{ return "Wolframe Daemon"; }
	const char* defaultServiceDescription() const		{ return "a daemon for wolframeing"; }
#endif // defined( _WIN32 )
};

static EchoAppProperties g_appProperties;

#if defined(_WIN32)		// we are on Windows

	int _Wolframe_winMain( int argc, char* argv[], const _Wolframe::AppProperties* );

	int main( int argc, char* argv[])
	{
		return ( _Wolframe_winMain( argc, argv, &g_appProperties ));
	}

#else			// we are on a POSIX system

	int _Wolframe_posixMain( int argc, char* argv[], const _Wolframe::AppProperties* );

	int main( int argc, char* argv[] )
	{
		return( _Wolframe_posixMain( argc, argv, &g_appProperties ));
	}
#endif // !defined(_WIN32)

