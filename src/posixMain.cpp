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

int _Wolframe_posixMain( int argc, char* argv[] )
{
// default logger (to console, warning level)
	_Wolframe::log::LogBackend::instance().setConsoleLevel( _Wolframe::log::LogLevel::LOGLEVEL_WARNING );

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
		_Wolframe::ApplicationInfo& appInfo = _Wolframe::ApplicationInfo::instance();
		appInfo.version( _Wolframe::Version( _Wolframe::applicationVersion() ));

		_Wolframe::config::CmdLineConfig   cmdLineCfg;
		const char *configFile;

		if ( !cmdLineCfg.parse( argc, argv ))	{	// there was an error parsing the command line
			LOG_ERROR << cmdLineCfg.errMsg();
			std::cerr << std::endl;
			cmdLineCfg.usage( std::cerr );
			std::cerr << std::endl;
			return _Wolframe::ErrorCode::FAILURE;
		}
// command line has been parsed successfully
// reset log level to the command line one, if specified
		if ( cmdLineCfg.debugLevel != _Wolframe::log::LogLevel::LOGLEVEL_UNDEFINED )
			_Wolframe::log::LogBackend::instance().setConsoleLevel( cmdLineCfg.debugLevel );

// if cmdLineCfg.errMsg() is not empty than we have a warning
		if ( !cmdLineCfg.errMsg().empty() )	{	// there was a warning parsing the command line
			LOG_WARNING << cmdLineCfg.errMsg();
		}

// if we have to print the version or the help do it and exit
		if ( cmdLineCfg.command == _Wolframe::config::CmdLineConfig::PRINT_VERSION )	{
			std::cout << std::endl << _Wolframe::applicationName() << gettext( " version " )
				  << appInfo.version().toString() << std::endl << std::endl;
			return _Wolframe::ErrorCode::OK;
		}
		if ( cmdLineCfg.command == _Wolframe::config::CmdLineConfig::PRINT_HELP )	{
			std::cout << std::endl << _Wolframe::applicationName() << gettext( " version " )
				  << appInfo.version().toString() << std::endl;
			cmdLineCfg.usage( std::cout );
			std::cout << std::endl;
			return _Wolframe::ErrorCode::OK;
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
			return _Wolframe::ErrorCode::FAILURE;
		}
		if ( ! boost::filesystem::is_regular_file( configFile ))	{
			LOG_FATAL << "'" << configFile << "' is not a regular file";
			return _Wolframe::ErrorCode::FAILURE;
		}

		std::string configurationPath = boost::filesystem::path( configFile).branch_path().string();

		_Wolframe::module::ModulesDirectory modDir( configurationPath);
		_Wolframe::config::ApplicationConfiguration conf;

		_Wolframe::config::ApplicationConfiguration::ConfigFileType cfgType =
				_Wolframe::config::ApplicationConfiguration::fileType( configFile, cmdLineCfg.cfgType );
		if ( cfgType == _Wolframe::config::ApplicationConfiguration::CONFIG_UNDEFINED )
			return _Wolframe::ErrorCode::FAILURE;
		if ( !conf.parseModules( configFile, cfgType ))
			return _Wolframe::ErrorCode::FAILURE;
		if ( ! modDir.loadModules( conf.moduleList() ))
			return _Wolframe::ErrorCode::FAILURE;
		conf.addModules( &modDir );
		if ( !conf.parse( configFile, cfgType ))
			return _Wolframe::ErrorCode::FAILURE;

// configuration file has been parsed successfully
// finalize the application configuration
		conf.finalize( cmdLineCfg );

// now here we know where to log to on stderr
		_Wolframe::log::LogBackend::instance().setConsoleLevel( conf.loggerCfg->stderrLogLevel );

// Check the configuration
		if ( cmdLineCfg.command == _Wolframe::config::CmdLineConfig::CHECK_CONFIG )	{
			std::cout << _Wolframe::applicationName() << gettext( " version " )
				  << appInfo.version().toString() << std::endl;
			if ( conf.check() )	{
				std::cout << "Configuration OK" << std::endl << std::endl;
				return _Wolframe::ErrorCode::OK;
			}
			else	{
				return _Wolframe::ErrorCode::FAILURE;
			}
		}

		if ( cmdLineCfg.command == _Wolframe::config::CmdLineConfig::PRINT_CONFIG )	{
			std::cout << std::endl << _Wolframe::applicationName() << gettext( " version " )
				  << appInfo.version().toString() << std::endl;
			conf.print( std::cout );
			std::cout << std::endl;
			return _Wolframe::ErrorCode::OK;
		}

		if ( cmdLineCfg.command == _Wolframe::config::CmdLineConfig::TEST_CONFIG )	{
			std::cout << "Not implemented yet" << std::endl << std::endl;
			return _Wolframe::ErrorCode::OK;
		}

		// Check the configuration before starting the service
		if ( !conf.check() )	{
			std::cout << std::endl << "Daemon not started because of a configuration ERROR"
				  << std::endl << std::endl;
			return _Wolframe::ErrorCode::FAILURE;
		}

		// Daemon stuff
		if( !conf.foreground ) {
			// Aba: maybe also in the foreground?
			// try to lock the pidfile, bail out if not possible
			if( boost::filesystem::exists( conf.serviceCfg->pidFile ) ) {
				boost::interprocess::file_lock lock( conf.serviceCfg->pidFile.c_str( ) );
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
			_Wolframe::log::LogBackend::instance().setSyslogLevel( conf.loggerCfg->syslogLogLevel );
			_Wolframe::log::LogBackend::instance().setSyslogFacility( conf.loggerCfg->syslogFacility );
			_Wolframe::log::LogBackend::instance().setSyslogIdent( conf.loggerCfg->syslogIdent );

			// if we are root we can drop privileges now
			struct group *groupent;
			struct passwd *passwdent;

			if ( !conf.serviceCfg->user.empty() )	{
				groupent = getgrnam( conf.serviceCfg->group.c_str( ) );
				if( groupent == NULL ) {
					LOG_CRITICAL << "Illegal group '" << conf.serviceCfg->group << "': " << _Wolframe::log::LogError::LogStrerror;
					return _Wolframe::ErrorCode::FAILURE;
				}
				LOG_TRACE << "changing group to " << conf.serviceCfg->group << "(" << groupent->gr_gid << ")";
				if( setgid( groupent->gr_gid ) < 0 ) {
					LOG_CRITICAL << "setgid for group '" << conf.serviceCfg->group << "' failed: " << _Wolframe::log::LogError::LogStrerror;
					return _Wolframe::ErrorCode::FAILURE;
				}

				passwdent = getpwnam( conf.serviceCfg->user.c_str( ) );
				if( passwdent == NULL ) {
					LOG_CRITICAL << "Illegal user '" << conf.serviceCfg->user << "': " << _Wolframe::log::LogError::LogStrerror;
					return _Wolframe::ErrorCode::FAILURE;
				}
				LOG_TRACE << "Changing user to " << conf.serviceCfg->user << "(" << passwdent->pw_uid << ")";
				if( setuid( passwdent->pw_uid ) < 0 ) {
					LOG_CRITICAL << "setuid for user '" << conf.serviceCfg->user << "' failed: " << _Wolframe::log::LogError::LogStrerror;
					return _Wolframe::ErrorCode::FAILURE;
				}
			}

			// create a pid file and lock id
			std::ofstream pidFile( conf.serviceCfg->pidFile.c_str( ), std::ios_base::trunc );
			if( !pidFile.good( ) ) {
				LOG_CRITICAL << "Unable to create PID file '" << conf.serviceCfg->pidFile << "'!";
				return _Wolframe::ErrorCode::FAILURE;
			}
			pidFile << getpid( ) << std::endl;
			pidFile.close( );

			// Create the final logger based on the configuration
			// file logger only here to get the right permissions
			_Wolframe::log::LogBackend::instance().setLogfileLevel( conf.loggerCfg->logFileLogLevel );
			_Wolframe::log::LogBackend::instance().setLogfileName( conf.loggerCfg->logFile );
		}

		// Block all signals for background thread.
		sigset_t new_mask;
		sigfillset( &new_mask );
		sigset_t old_mask;
		pthread_sigmask( SIG_BLOCK, &new_mask, &old_mask );

		LOG_NOTICE << "Starting server";

		// Run server in background thread(s).
		_Wolframe::ServerHandler handler( conf.handlerCfg, &modDir );
		_Wolframe::net::server s( conf.serverCfg, handler );
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
		if( !conf.foreground ) {
			(void)remove( conf.serviceCfg->pidFile.c_str( ) );
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
