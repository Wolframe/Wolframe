/************************************************************************

 Copyright (C) 2011 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
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
#include "standardConfigs.hpp"
#include "server.hpp"
#include "ErrorCodes.hpp"
#include "logger.hpp"
#include "appSingleton.hpp"

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


int _Wolframe_posixMain( int argc, char* argv[] )
{
// i18n global stuff
	if ( setlocale( LC_ALL, "" ) == NULL )	{
		LOG_ERROR << "Unable to set locale. Falling back to default.";
	}
	else	{
		if ( bindtextdomain( "Wolframe", "../po" ) == NULL )	{
			LOG_FATAL << "Not enough memory to bind textdomain";
			return _Wolframe::ErrorCodes::FAILURE;
		}
		if ( textdomain( "Wolframe" ) == NULL )	{
			LOG_FATAL << "Not enough memory to set textdomain";
			return _Wolframe::ErrorCodes::FAILURE;
		}
	}
// end of i18n global stuff

	try	{
//		_Wolframe::Version	appVersion( _Wolframe::applicationMajorVersion(),
//						    _Wolframe::applicationMinorVersion(),
//						    _Wolframe::applicationRevisionVersion(),
//						    _Wolframe::applicationBuildVersion() );
		_Wolframe::ApplicationSingleton& appSingleton = _Wolframe::ApplicationSingleton::instance();
		appSingleton.version( _Wolframe::Version( _Wolframe::applicationMajorVersion(),
							  _Wolframe::applicationMinorVersion(),
							  _Wolframe::applicationRevisionVersion(),
							  _Wolframe::applicationBuildVersion() ));
		_Wolframe::config::CmdLineConfig   cmdLineCfg;
		const char *configFile;

		if ( !cmdLineCfg.parse( argc, argv ))	{	// there was an error parsing the command line
			LOG_ERROR << cmdLineCfg.errMsg();
			std::cerr << std::endl;
			cmdLineCfg.usage( std::cerr );
			std::cerr << std::endl;
			return _Wolframe::ErrorCodes::FAILURE;
		}
// command line has been parsed successfully
// if cmdLineCfg.errMsg() is not empty than we have a warning
		if ( !cmdLineCfg.errMsg().empty() )	// there was a warning parsing the command line
			LOG_ERROR << cmdLineCfg.errMsg();

// if we have to print the version or the help do it and exit
		if ( cmdLineCfg.command == _Wolframe::config::CmdLineConfig::PRINT_VERSION )	{
			std::cout << std::endl << gettext( "BOBOBO version " )
				  << appSingleton.version().toString() << std::endl << std::endl;
			return _Wolframe::ErrorCodes::OK;
		}
		if ( cmdLineCfg.command == _Wolframe::config::CmdLineConfig::PRINT_HELP )	{
			std::cout << std::endl << _Wolframe::applicationName() << gettext( "BOBOBO version " )
				  << appSingleton.version().toString() << std::endl;
			cmdLineCfg.usage( std::cout );
			std::cout << std::endl;
			return _Wolframe::ErrorCodes::OK;
		}

// decide what configuration file to use
		if ( !cmdLineCfg.cfgFile.empty() )	// if it has been specified than that's The One ! (and only)
			configFile = cmdLineCfg.cfgFile.c_str();
		else
			configFile = _Wolframe::config::ApplicationConfiguration::chooseFile( _Wolframe::config::defaultMainConfig(),
												  _Wolframe::config::defaultUserConfig(),
												  _Wolframe::config::defaultLocalConfig() );
		if ( configFile == NULL )	{	// there is no configuration file
			LOG_FATAL << gettext ( "no configuration file found !" );
			return _Wolframe::ErrorCodes::FAILURE;
		}

		_Wolframe::config::ApplicationConfiguration config;

		if ( !config.parse( configFile ))	// there was an error parsing the configuration file
			return _Wolframe::ErrorCodes::FAILURE;

// configuration file has been parsed successfully
// finalize the application configuration
		config.finalize( cmdLineCfg );

// now here we know where to log to on stderr
		_Wolframe::log::LogBackend::instance().setConsoleLevel( config.loggerConf->stderrLogLevel );

// Check the configuration
		if ( cmdLineCfg.command == _Wolframe::config::CmdLineConfig::CHECK_CONFIG )	{
			std::cout << gettext( "BOBOBO version " ) << appSingleton.version().toString() << std::endl;
			if ( config.check() )	{
				std::cout << "Configuration OK" << std::endl << std::endl;
				return _Wolframe::ErrorCodes::OK;
			}
			else	{
				return _Wolframe::ErrorCodes::FAILURE;
			}
		}

		if ( cmdLineCfg.command == _Wolframe::config::CmdLineConfig::PRINT_CONFIG )	{
			std::cout << std::endl << gettext( "BOBOBO version " ) << appSingleton.version().toString() << std::endl;
			config.print( std::cout );
			std::cout << std::endl;
			return _Wolframe::ErrorCodes::OK;
		}

		if ( cmdLineCfg.command == _Wolframe::config::CmdLineConfig::TEST_CONFIG )	{
			std::cout << "Not implemented yet" << std::endl << std::endl;
			return _Wolframe::ErrorCodes::OK;
		}

		// Daemon stuff
		if( !config.foreground ) {
			// Aba: maybe also in the foreground?
			// try to lock the pidfile, bail out if not possible
			if( boost::filesystem::exists( config.serviceConf->pidFile ) ) {
				boost::interprocess::file_lock lock( config.serviceConf->pidFile.c_str( ) );
				if( lock.try_lock( ) ) {
					LOG_ERROR << "Pidfile is locked, another daemon running?";
					return _Wolframe::ErrorCodes::FAILURE;
				}
			}

			// daemonize, lose process group, terminal output, etc.
			if( daemon( 0, 0 ) ) {
				LOG_CRITICAL << "Daemonizing server failed: " << _Wolframe::log::LogError::LogStrerror;
				return _Wolframe::ErrorCodes::FAILURE;
			}

			// now here we lost constrol over the console, we should
			// create a temporary logger which at least tells what's
			// going on in the syslog
			_Wolframe::log::LogBackend::instance().setSyslogLevel( config.loggerConf->syslogLogLevel );
			_Wolframe::log::LogBackend::instance().setSyslogFacility( config.loggerConf->syslogFacility );
			_Wolframe::log::LogBackend::instance().setSyslogIdent( config.loggerConf->syslogIdent );

			// if we are root we can drop privileges now
			struct group *groupent;
			struct passwd *passwdent;

			passwdent = getpwnam( config.serviceConf->user.c_str( ) );
			if( passwdent == NULL ) {
				LOG_CRITICAL << "Illegal user '" << config.serviceConf->user << "': " << _Wolframe::log::LogError::LogStrerror;
				return _Wolframe::ErrorCodes::FAILURE;
			}

			groupent = getgrnam( config.serviceConf->group.c_str( ) );
			if( groupent == NULL ) {
				LOG_CRITICAL << "Illegal group '" << config.serviceConf->group << "': " << _Wolframe::log::LogError::LogStrerror;
				return _Wolframe::ErrorCodes::FAILURE;
			}

			if( setgid( groupent->gr_gid ) < 0 ) {
				LOG_CRITICAL << "setgid for group '" << config.serviceConf->group << "' failed: " << _Wolframe::log::LogError::LogStrerror;
				return _Wolframe::ErrorCodes::FAILURE;
			}

			if( setuid( passwdent->pw_uid ) < 0 ) {
				LOG_CRITICAL << "setgid for user '" << config.serviceConf->user << "' failed: " << _Wolframe::log::LogError::LogStrerror;
				return _Wolframe::ErrorCodes::FAILURE;
			}

			// create a pid file and lock id
			std::ofstream pidFile( config.serviceConf->pidFile.c_str( ), std::ios_base::trunc );
			if( !pidFile.good( ) ) {
				LOG_CRITICAL << "Unable to create PID file '" << config.serviceConf->pidFile << "'!";
				return _Wolframe::ErrorCodes::FAILURE;
			}
			pidFile << getpid( ) << std::endl;
			pidFile.close( );

			// Create the final logger based on the configuration
			// file logger only here to get the right permissions
			_Wolframe::log::LogBackend::instance().setLogfileLevel( config.loggerConf->logFileLogLevel );
			_Wolframe::log::LogBackend::instance().setLogfileName( config.loggerConf->logFile );
		}

		// Block all signals for background thread.
		sigset_t new_mask;
		sigfillset( &new_mask );
		sigset_t old_mask;
		pthread_sigmask( SIG_BLOCK, &new_mask, &old_mask );

		LOG_NOTICE << "Starting server";

		// Run server in background thread(s).
		_Wolframe::ServerHandler handler( config.handlerConf );
		_Wolframe::net::server s( config.serverConf, handler );
		boost::thread t( boost::bind( &_Wolframe::net::server::run, &s ));

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
			(void)remove( config.serviceConf->pidFile.c_str( ) );
		}
	}
	catch (std::exception& e)	{
		// Aba: how to delete the pid file here?
		LOG_ERROR << "posixMain: exception: " << e.what() << "\n";
		return _Wolframe::ErrorCodes::FAILURE;
	}

	return _Wolframe::ErrorCodes::OK;
}

#endif // !defined(_WIN32)
