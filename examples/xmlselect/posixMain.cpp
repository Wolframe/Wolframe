//
// posixMain.cpp
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "version.hpp"
#include "commandLine.hpp"
#include "configFile.hpp"
#include "appConfig.hpp"
#include "server.hpp"
#include "ErrorCodes.hpp"
#include "logger.hpp"

#include "xmlselectHandler.hpp"

#include <libintl.h>
#include <locale.h>

#if defined(_WIN32)
#error "This is the POSIX main !"
#else

#include <pthread.h>
#include <signal.h>
#include <unistd.h>

// daemon stuff
#include <fstream>
#include <cstdio>
#include <grp.h>
#include <pwd.h>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/filesystem.hpp>

static const unsigned short MAJOR_VERSION = 0;
static const short unsigned MINOR_VERSION = 0;
static const short unsigned REVISION_NUMBER = 3;

static const char *DEFAULT_MAIN_CONFIG = "/etc/smerp.conf";
static const char *DEFAULT_USER_CONFIG = "~/smerp.conf";
static const char *DEFAULT_LOCAL_CONFIG = "./smerp.conf";

_SMERP::LogBackend	logBack;

int _SMERP_posixMain( int argc, char* argv[] )
{
// i18n global stuff
	if ( setlocale( LC_ALL, "" ) == NULL )	{
		std::cerr << "Unable to set locale. Falling back to default." << std::endl;
	}
	else	{
		if ( bindtextdomain( "SMERP", "../po" ) == NULL )	{
			std::cerr << "Not enough memory to bind textdomain" << std::endl;
			return _SMERP::ErrorCodes::FAILURE;
		}
		if ( textdomain( "SMERP" ) == NULL )	{
			std::cerr << "Not enough memory to set textdomain" << std::endl;
			return _SMERP::ErrorCodes::FAILURE;
		}
	}
// end of i18n global stuff

	try	{
		_SMERP::Version		appVersion( MAJOR_VERSION, MINOR_VERSION, REVISION_NUMBER );
		_SMERP::CmdLineConfig	cmdLineCfg;
		const char		*configFile;

		if ( !cmdLineCfg.parse( argc, argv ))	{	// there was an error parsing the command line
			std::cerr << cmdLineCfg.errMsg() << std::endl << std::endl;
			cmdLineCfg.usage( std::cerr );
			std::cerr << std::endl;
			return _SMERP::ErrorCodes::FAILURE;
		}
// command line has been parsed successfully
// if cmdLineCfg.errMsg() is not empty than we have a warning
		if ( !cmdLineCfg.errMsg().empty() )	// there was a warning parsing the command line
			std::cerr << "BOO:" << cmdLineCfg.errMsg() << std::endl << std::endl;

// if we have to print the version or the help do it and exit
		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::PRINT_VERSION )	{
			std::cout << std::endl << gettext( "BOBOBO version " )
				<< appVersion.toString() << std::endl << std::endl;
			return _SMERP::ErrorCodes::OK;
		}
		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::PRINT_HELP )	{
			std::cout << std::endl << gettext( "BOBOBO version " )
				<< appVersion.toString() << std::endl;
			cmdLineCfg.usage( std::cout );
			std::cout << std::endl;
			return _SMERP::ErrorCodes::OK;
		}

// decide what configuration file to use
		if ( !cmdLineCfg.cfgFile.empty() )	// if it has been specified than that's The One ! (and only)
			configFile = cmdLineCfg.cfgFile.c_str();
		else
			configFile = _SMERP::CfgFileConfig::chooseFile( DEFAULT_MAIN_CONFIG,
								       DEFAULT_USER_CONFIG,
								       DEFAULT_LOCAL_CONFIG );
		if ( configFile == NULL )	{	// there is no configuration file
			std::cerr << gettext ( "MOMOMO: no configuration file found !" ) << std::endl << std::endl;
			return _SMERP::ErrorCodes::FAILURE;
		}

		_SMERP::CfgFileConfig	cfgFileCfg;
		if ( !cfgFileCfg.parse( configFile ))	{	// there was an error parsing the configuration file
			std::cerr << cfgFileCfg.errMsg() << std::endl << std::endl;
			return _SMERP::ErrorCodes::FAILURE;
		}
		else if ( !cfgFileCfg.errMsg().empty())
			std::cerr << cfgFileCfg.errMsg() << std::endl;
// configuration file has been parsed successfully
// build the application configuration
		_SMERP::ApplicationConfiguration config( cmdLineCfg, cfgFileCfg);

// now here we know where to log to on stderr
		logBack.setConsoleLevel( config.stderrLogLevel );

// Check the configuration
		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::CHECK_CONFIG )	{
			std::cout << std::endl << gettext( "BOBOBO version " )
				<< appVersion.toString() << std::endl;
			if ( config.check() )	{
				if ( config.errMsg().empty() )	{
					std::cout << "Configuration OK" << std::endl << std::endl;
					return _SMERP::ErrorCodes::OK;
				}
				else	{
					std::cout << "WARNING: " << config.errMsg() << std::endl << std::endl;
					return _SMERP::ErrorCodes::OK;
				}
			}
			else	{
				std::cout << "ERROR: " << config.errMsg() << std::endl << std::endl;
				return _SMERP::ErrorCodes::OK;
			}
		}

		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::PRINT_CONFIG )	{
			std::cout << std::endl << gettext( "BOBOBO version " )
				<< appVersion.toString() << std::endl;
			config.print( std::cout );
			std::cout << std::endl;
			return _SMERP::ErrorCodes::OK;
		}

		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::TEST_CONFIG )	{
			std::cout << "Not implemented yet" << std::endl << std::endl;
			return _SMERP::ErrorCodes::OK;
		}

		// Daemon stuff
		if( !config.foreground ) {
			// Aba: maybe also in the foreground?
			// try to lock the pidfile, bail out if not possible
			if( boost::filesystem::exists( config.pidFile ) ) {
				boost::interprocess::file_lock lock( config.pidFile.c_str( ) );
				if( lock.try_lock( ) ) {
					std::cerr << "Pidfile is locked, another daemon running?" << std::endl;
					return _SMERP::ErrorCodes::FAILURE;
				}
			}

			// daemonize, lose process group, terminal output, etc.
			if( daemon( 0, 0 ) ) {
				std::cerr << "Going to daemon mode failed" << std::endl;
				return _SMERP::ErrorCodes::FAILURE;
			}

			// now here we lost constrol over the console, we should
			// create a temporary logger which at least tells what's
			// going on in the syslog
			logBack.setSyslogLevel( config.syslogLogLevel );
			logBack.setSyslogFacility( config.syslogFacility );
			logBack.setSyslogIdent( config.syslogIdent );

			// if we are root we can drop privileges now
			struct group *groupent;
			struct passwd *passwdent;

			groupent = getgrnam( config.group.c_str( ) );
			passwdent = getpwnam( config.user.c_str( ) );
			if( groupent == NULL || passwdent == NULL ) {
				LOG_CRITICAL << "Illegal group '" << config.group << "' or user '" << config.user << "'";
				return _SMERP::ErrorCodes::FAILURE;
			}

			if( setgid( groupent->gr_gid ) < 0 ) {
				LOG_CRITICAL << "setgid for group '" << config.group << "' failed!";
				return _SMERP::ErrorCodes::FAILURE;
			}

			if( setuid( passwdent->pw_uid ) < 0 ) {
				LOG_CRITICAL << "setgid for user '" << config.user << "' failed!";
				return _SMERP::ErrorCodes::FAILURE;
			}

			// create a pid file and lock id
			std::ofstream pidFile( config.pidFile.c_str( ), std::ios_base::trunc );
			if( !pidFile.good( ) ) {
				LOG_CRITICAL << "Unable to create PID file '" << config.pidFile << "'!";
				return _SMERP::ErrorCodes::FAILURE;
			}
			pidFile << getpid( ) << std::endl;
			pidFile.close( );

			// Create the final logger based on the configuration
			// file logger only here to get the right permissions
			logBack.setLogfileLevel( config.logFileLogLevel );
			logBack.setLogfileName( config.logFile );
		}

		// Block all signals for background thread.
		sigset_t new_mask;
		sigfillset( &new_mask );
		sigset_t old_mask;
		pthread_sigmask( SIG_BLOCK, &new_mask, &old_mask );

		LOG_NOTICE << "Starting server";

		// Run server in background thread(s).
		_SMERP::xmlselect::Server	echo;
		_SMERP::Network::server s( config.address, config.SSLaddress, echo,
					   config.threads, config.maxConnections );
		boost::thread t( boost::bind( &_SMERP::Network::server::run, &s ));

		// Restore previous signals.
		pthread_sigmask( SIG_SETMASK, &old_mask, 0 );

		// Wait for signal indicating time to shut down.
		sigset_t wait_mask;
		sigemptyset( &wait_mask );
		sigaddset( &wait_mask, SIGINT );
		sigaddset( &wait_mask, SIGQUIT );
		sigaddset( &wait_mask, SIGTERM );
		pthread_sigmask( SIG_BLOCK, &wait_mask, 0 );
		int sig = 0;
		sigwait( &wait_mask, &sig );

		// Stop the server.
		LOG_INFO << "Stopping server";
		s.stop();
		t.join();
		LOG_NOTICE << "Server stopped";

		// Daemon stuff
		if( !config.foreground ) {
			(void)remove( config.pidFile.c_str( ) );
		}
	}
	catch (std::exception& e)	{
		// Aba: how to delete the pid file here?
		LOG_ERROR << "posixMain: exception: " << e.what() << "\n";
		return _SMERP::ErrorCodes::FAILURE;
	}

	return _SMERP::ErrorCodes::OK;
}

#endif // !defined(_WIN32)
