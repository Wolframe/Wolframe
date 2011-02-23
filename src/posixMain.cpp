//
// posixMain.cpp
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "appProperties.hpp"

#include "version.hpp"
#include "commandLine.hpp"
#include "appConfig.hpp"
#include "configStandard.hpp"
#include "server.hpp"
#include "ErrorCodes.hpp"
#include "logger.hpp"

#include "connectionHandler.hpp"

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
#include <sstream>
#include <cstdio>
#include <grp.h>
#include <pwd.h>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/filesystem.hpp>


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
		_SMERP::Version		appVersion( _SMERP::applicationMajorVersion(),
						    _SMERP::applicationMinorVersion(),
						    _SMERP::applicationRevisionVersion(),
						    _SMERP::applicationBuildVersion() );
		_SMERP::Configuration::CmdLineConfig   cmdLineCfg;
		const char *configFile;

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
		if ( cmdLineCfg.command == _SMERP::Configuration::CmdLineConfig::PRINT_VERSION )	{
			std::cout << std::endl << gettext( "BOBOBO version " )
				<< appVersion.toString() << std::endl << std::endl;
			return _SMERP::ErrorCodes::OK;
		}
		if ( cmdLineCfg.command == _SMERP::Configuration::CmdLineConfig::PRINT_HELP )	{
			std::cout << std::endl << _SMERP::applicationName() << gettext( "BOBOBO version " )
				<< appVersion.toString() << std::endl;
			cmdLineCfg.usage( std::cout );
			std::cout << std::endl;
			return _SMERP::ErrorCodes::OK;
		}

// decide what configuration file to use
		if ( !cmdLineCfg.cfgFile.empty() )	// if it has been specified than that's The One ! (and only)
			configFile = cmdLineCfg.cfgFile.c_str();
		else
			configFile = _SMERP::Configuration::ApplicationConfiguration::chooseFile( _SMERP::Configuration::defaultMainConfig(),
												  _SMERP::Configuration::defaultUserConfig(),
												  _SMERP::Configuration::defaultLocalConfig() );
		if ( configFile == NULL )	{	// there is no configuration file
			std::cerr << gettext ( "MOMOMO: no configuration file found !" ) << std::endl << std::endl;
			return _SMERP::ErrorCodes::FAILURE;
		}

		_SMERP::Configuration::ApplicationConfiguration config;
		std::stringstream errMsg;

		if ( !config.parse( configFile, errMsg ))	{	// there was an error parsing the configuration file
			std::cerr << errMsg.str() << std::endl << std::endl;
			return _SMERP::ErrorCodes::FAILURE;
		}
		else if ( ! errMsg.str().empty() )
			std::cerr << errMsg.str() << std::endl;
// configuration file has been parsed successfully
// finalize the application configuration
		config.finalize( cmdLineCfg );


// now here we know where to log to on stderr
		_SMERP::LogBackend::instance().setConsoleLevel( config.logConfig->stderrLogLevel );

// Check the configuration
		if ( cmdLineCfg.command == _SMERP::Configuration::CmdLineConfig::CHECK_CONFIG )	{
			std::cout << std::endl << gettext( "BOBOBO version " )
				<< appVersion.toString() << std::endl;
			if ( config.check( errMsg ) )	{
				if ( errMsg.str().empty() )	{
					std::cout << "Configuration OK" << std::endl << std::endl;
					return _SMERP::ErrorCodes::OK;
				}
				else	{
					std::cout << "WARNING: " << errMsg.str() << std::endl << std::endl;
					return _SMERP::ErrorCodes::OK;
				}
			}
			else	{
				std::cout << "ERROR: " << errMsg.str() << std::endl << std::endl;
				return _SMERP::ErrorCodes::OK;
			}
		}

		if ( cmdLineCfg.command == _SMERP::Configuration::CmdLineConfig::PRINT_CONFIG )	{
			std::cout << std::endl << gettext( "BOBOBO version " )
				<< appVersion.toString() << std::endl;
			config.print( std::cout );
			std::cout << std::endl;
			return _SMERP::ErrorCodes::OK;
		}

		if ( cmdLineCfg.command == _SMERP::Configuration::CmdLineConfig::TEST_CONFIG )	{
			std::cout << "Not implemented yet" << std::endl << std::endl;
			return _SMERP::ErrorCodes::OK;
		}

		// Daemon stuff
		if( !config.foreground ) {
			// Aba: maybe also in the foreground?
			// try to lock the pidfile, bail out if not possible
			if( boost::filesystem::exists( config.srvConfig->pidFile ) ) {
				boost::interprocess::file_lock lock( config.srvConfig->pidFile.c_str( ) );
				if( lock.try_lock( ) ) {
					std::cerr << "Pidfile is locked, another daemon running?" << std::endl;
					return _SMERP::ErrorCodes::FAILURE;
				}
			}

			// daemonize, lose process group, terminal output, etc.
			if( daemon( 0, 0 ) ) {
				std::cerr << "Daemonizing server failed" << std::endl;
				return _SMERP::ErrorCodes::FAILURE;
			}

			// now here we lost constrol over the console, we should
			// create a temporary logger which at least tells what's
			// going on in the syslog
			_SMERP::LogBackend::instance().setSyslogLevel( config.logConfig->syslogLogLevel );
			_SMERP::LogBackend::instance().setSyslogFacility( config.logConfig->syslogFacility );
			_SMERP::LogBackend::instance().setSyslogIdent( config.logConfig->syslogIdent );

			// if we are root we can drop privileges now
			struct group *groupent;
			struct passwd *passwdent;

			groupent = getgrnam( config.srvConfig->group.c_str( ) );
			passwdent = getpwnam( config.srvConfig->user.c_str( ) );
			if( groupent == NULL || passwdent == NULL ) {
				LOG_CRITICAL << "Illegal group '" << config.srvConfig->group << "' or user '" << config.srvConfig->user << "'";
				return _SMERP::ErrorCodes::FAILURE;
			}

			if( setgid( groupent->gr_gid ) < 0 ) {
				LOG_CRITICAL << "setgid for group '" << config.srvConfig->group << "' failed!";
				return _SMERP::ErrorCodes::FAILURE;
			}

			if( setuid( passwdent->pw_uid ) < 0 ) {
				LOG_CRITICAL << "setgid for user '" << config.srvConfig->user << "' failed!";
				return _SMERP::ErrorCodes::FAILURE;
			}

			// create a pid file and lock id
			std::ofstream pidFile( config.srvConfig->pidFile.c_str( ), std::ios_base::trunc );
			if( !pidFile.good( ) ) {
				LOG_CRITICAL << "Unable to create PID file '" << config.srvConfig->pidFile << "'!";
				return _SMERP::ErrorCodes::FAILURE;
			}
			pidFile << getpid( ) << std::endl;
			pidFile.close( );

			// Create the final logger based on the configuration
			// file logger only here to get the right permissions
			_SMERP::LogBackend::instance().setLogfileLevel( config.logConfig->logFileLogLevel );
			_SMERP::LogBackend::instance().setLogfileName( config.logConfig->logFile );
		}

		// Block all signals for background thread.
		sigset_t new_mask;
		sigfillset( &new_mask );
		sigset_t old_mask;
		pthread_sigmask( SIG_BLOCK, &new_mask, &old_mask );

		LOG_NOTICE << "Starting server";

		// Run server in background thread(s).
		_SMERP::ServerHandler	handler( config.handlerConfig );
		_SMERP::Network::server s( config.srvConfig->address, config.srvConfig->SSLaddress, handler,
					   config.srvConfig->threads, config.srvConfig->maxConnections );
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
			(void)remove( config.srvConfig->pidFile.c_str( ) );
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
