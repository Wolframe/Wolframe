/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
#include "wolframedCommandLine.hpp"
#include "appConfig.hpp"
#include "standardConfigs.hpp"
#include "server.hpp"
#include "system/errorCode.hpp"
#include "logger-v1.hpp"
#include "appInfo.hpp"
#include "service.hpp"
#include "module/moduleDirectory.hpp"

#include "system/connectionHandler.hpp"

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
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

// Solaris has no BSD daemon function, provide our own
#ifdef SUNOS
int daemon( int nochdir, int noclose )
{
	switch( fork( ) ) {
		case -1:
			// fork error
			return -1;

		case 0:
			// the new daemon
			break;

		default:
			// terminate parent without closing file descriptors
			_exit( 0 );
	}

	// new process group
	if( setsid( ) == -1 ) return -1;

	// optionally change to root dir (avoid unmount problems)
	if( !nochdir ) chdir( "/" );

	// assign /dev/null to stdin, stdout and stderr
	if( !noclose ) {
		int fd = open( "/dev/null", O_RDWR, 0 );
		if( fd != -1 ) {
			(void)dup2( fd, STDIN_FILENO );
			(void)dup2( fd, STDOUT_FILENO );
			(void)dup2( fd, STDERR_FILENO );
		}
	}

	return 0;
}
#endif

int _Wolframe_posixMain( _Wolframe::ServiceInterface* service, int argc, char* argv[])
{
	const _Wolframe::AppProperties* appProperties = service->appProperties();
// default logger (to console, warning level)
	_Wolframe::log::LogBackend::instance().setConsoleLevel( _Wolframe::log::LogLevel::LOGLEVEL_WARNING );

	if (!service->loadConfig( argc, argv))
	{
		LOG_FATAL << "Failed to load configuration";
		return _Wolframe::ErrorCode::FAILURE;
	}

// i18n global stuff
	if ( setlocale( LC_ALL, "" ) == NULL )	{
		LOG_ERROR << "Unable to set locale. Falling back to default.";
	}
	else	{
		if ( bindtextdomain( "Wolframe", "../po" ) == NULL )	{
			LOG_FATAL << "Not enough memory to bind textdomain";
			return _Wolframe::ErrorCode::FAILURE;
		}
		if ( textdomain( "Wolframe" ) == NULL )	{
			LOG_FATAL << "Not enough memory to set textdomain";
			return _Wolframe::ErrorCode::FAILURE;
		}
	}
// end of i18n global stuff

	try	{
// now here we know where to log to on stderr
		_Wolframe::log::LogBackend::instance().setConsoleLevel( service->loggerConfig().stderrLogLevel );

// Check the configuration
		if ( std::strcmp( service->command(), "configcheck") == 0 )	{
			std::cout << appProperties->applicationName() << gettext( " version " )
				  << appProperties->applicationVersion().toString() << std::endl;
			if ( service->configcheck() )	{
				std::cout << "Configuration check OK" << std::endl << std::endl;
				return _Wolframe::ErrorCode::OK;
			}
			else	{
				return _Wolframe::ErrorCode::FAILURE;
			}
		}

		if ( std::strcmp( service->command(), "printconfig") == 0 )	{
			std::cout << std::endl << appProperties->applicationName() << gettext( " version " )
				  << appProperties->applicationVersion().toString() << std::endl;
			service->printconfig( std::cout );
			std::cout << std::endl;
			return _Wolframe::ErrorCode::OK;
		}

		if ( std::strcmp( service->command(), "configtest") == 0 )	{
			if ( service->configcheck() )	{
				std::cout << "Configuration test OK" << std::endl << std::endl;
				return _Wolframe::ErrorCode::OK;
			}
			else	{
				return _Wolframe::ErrorCode::FAILURE;
			}
		}

		// Check the configuration before starting the service
		if ( !service->configcheck() )	{
			std::cout << std::endl << "Daemon not started because of a configuration ERROR"
				  << std::endl << std::endl;
			return _Wolframe::ErrorCode::FAILURE;
		}

		// Daemon stuff
		if( !service->foreground() ) {
			// Aba: maybe also in the foreground?
			// try to lock the pidfile, bail out if not possible
			if( service->pidfile() && boost::filesystem::exists( service->pidfile()) ) {
				boost::interprocess::file_lock lock( service->pidfile());
				if( lock.try_lock( ) ) {
					LOG_ERROR << "Pidfile is locked, another daemon running?";
					return _Wolframe::ErrorCode::FAILURE;
				}
			}

			// daemonize, lose process group, terminal output, etc.
			if( daemon( 0, 0 ) ) {
				LOG_CRITICAL << "Daemonizing server failed: " << _Wolframe::log::LogError::LogStrerror;
				return _Wolframe::ErrorCode::FAILURE;
			}

			// now here we lost constrol over the console, we should
			// create a temporary logger which at least tells what's
			// going on in the syslog
			_Wolframe::log::LogBackend::instance().setSyslogLevel( service->loggerConfig().syslogLogLevel );
			_Wolframe::log::LogBackend::instance().setSyslogFacility( service->loggerConfig().syslogFacility );
			_Wolframe::log::LogBackend::instance().setSyslogIdent( service->loggerConfig().syslogIdent );

			// if we are root we can drop privileges now
			struct group *groupent;
			struct passwd *passwdent;

			if ( service->user() )	{
				groupent = service->group()?getgrnam( service->group()):NULL;
				if( groupent == NULL ) {
					LOG_CRITICAL << "Illegal group '" << (service->group()?service->group():"<undefined>") << "': " << _Wolframe::log::LogError::LogStrerror;
					return _Wolframe::ErrorCode::FAILURE;
				}
				LOG_TRACE << "changing group to " << service->group() << "(" << groupent->gr_gid << ")";
				if( setgid( groupent->gr_gid ) < 0 ) {
					LOG_CRITICAL << "setgid for group '" << service->group() << "' failed: " << _Wolframe::log::LogError::LogStrerror;
					return _Wolframe::ErrorCode::FAILURE;
				}

				passwdent = getpwnam( service->user());
				if( passwdent == NULL ) {
					LOG_CRITICAL << "Illegal user '" << service->user() << "': " << _Wolframe::log::LogError::LogStrerror;
					return _Wolframe::ErrorCode::FAILURE;
				}
				LOG_TRACE << "Changing user to " << service->user() << "(" << passwdent->pw_uid << ")";
				if( setuid( passwdent->pw_uid ) < 0 ) {
					LOG_CRITICAL << "setuid for user '" << service->user() << "' failed: " << _Wolframe::log::LogError::LogStrerror;
					return _Wolframe::ErrorCode::FAILURE;
				}
			}

			// create a pid file and lock id
			std::ofstream pidFile( service->pidfile(), std::ios_base::trunc );
			if( !pidFile.good( ) ) {
				LOG_CRITICAL << "Unable to create PID file '" << service->pidfile() << "'!";
				return _Wolframe::ErrorCode::FAILURE;
			}
			pidFile << getpid( ) << std::endl;
			pidFile.close( );

			// Create the final logger based on the configuration
			// file logger only here to get the right permissions
			_Wolframe::log::LogBackend::instance().setLogfileLevel( service->loggerConfig().logFileLogLevel );
			_Wolframe::log::LogBackend::instance().setLogfileName( service->loggerConfig().logFile );
		}

		// Block all signals for background thread.
		sigset_t new_mask;
		sigfillset( &new_mask );
		sigset_t old_mask;
		pthread_sigmask( SIG_BLOCK, &new_mask, &old_mask );

		LOG_NOTICE << "Starting server";

		if (!service->start())
		{
			return _Wolframe::ErrorCode::FAILURE;
		}

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
		service->stop();
		LOG_NOTICE << "Server stopped";

		// Daemon stuff
		if( !service->foreground() ) {
			(void)remove( service->pidfile());
		}
	}
	catch (std::exception& e)	{
		// Aba: how to delete the pid file here?
		LOG_FATAL << "posixMain: exception: " << e.what() << "\n";
		return _Wolframe::ErrorCode::FAILURE;
	}

	return _Wolframe::ErrorCode::OK;
}

#endif // !defined(_WIN32)
