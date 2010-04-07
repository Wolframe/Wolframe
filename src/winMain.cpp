//
// winMain.cpp
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "appInstance.hpp"
#include "commandLine.hpp"
#include "configFile.hpp"
#include "appConfig.hpp"
#include "server.hpp"
#include "ErrorCodes.hpp"
#include "logger.hpp"

#if !defined(_WIN32)
#error "This is the WIN32 main !"
#else

#include <WinSvc.h>

static const unsigned short MAJOR_VERSION = 0;
static const short unsigned MINOR_VERSION = 0;
static const short unsigned REVISION_NUMBER = 3;

static const int DEFAULT_DEBUG_LEVEL = 3;

static const int DEFAULT_SERVICE_TIMEOUT = 5000;

static const char *DEFAULT_SERVICE_NAME = "smerpd";

boost::function0<void> consoleCtrlFunction;

BOOL WINAPI consoleCtrlHandler(DWORD ctrlType)
{
	switch (ctrlType)	{
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			LOG_INFO << "Stopping server";
			consoleCtrlFunction();
			return TRUE;
		default:
			return FALSE;
	}
}

static void install_as_service( const _SMERP::ApplicationConfiguration& config )
{
// get service control manager
	SC_HANDLE scm = (SC_HANDLE)OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS );
	
// retrieve absolute path of binary
	TCHAR binary_path[MAX_PATH];
	DWORD res = GetModuleFileName( NULL, binary_path, MAX_PATH );
	
// create the service
	SC_HANDLE service = CreateService( scm,
		config.serviceName.c_str( ), config.serviceDisplayName.c_str( ),
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		binary_path, NULL, NULL, NULL, NULL, NULL );

// set description of the service
	SERVICE_DESCRIPTION descr;
	descr.lpDescription = (LPTSTR)config.serviceDescription.c_str( );
	(void)ChangeServiceConfig2( service, SERVICE_CONFIG_DESCRIPTION, &descr );

// TODO: add location of the configuration file to the registry

// free handles
	(void)CloseServiceHandle( service );
	(void)CloseServiceHandle( scm );
}

static void remove_as_service( const _SMERP::ApplicationConfiguration& config )
{
// get service control manager
	SC_HANDLE scm = (SC_HANDLE)OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS );

// get service handle of the service to delete (identified by service name)
	SC_HANDLE service = OpenService( scm, config.serviceName.c_str( ), SERVICE_ALL_ACCESS );

// remove the service
	(void)DeleteService( service );

// TODO: remove location of the configuration file to the registry

// free handles
	(void)CloseServiceHandle( service );
	(void)CloseServiceHandle( scm );
}

static SERVICE_STATUS_HANDLE serviceStatusHandle;
static HANDLE serviceStopEvent = NULL;

// events from SCM and Windows comes here. Be responsive!!
void WINAPI serviceCtrlHandler( DWORD control )
{
	char errbuf[512];
	BOOL res;

	wolf_log( WOLF_LOG_DEBUG, WOLF_CATEGORY_SERVICE, WOLF_MSG_SERVICE_HANDLING_EVENT,
		_( "service handler received status change '%s' (%d)" ),
		wolf_service_control_to_str( control ),
		control );

	switch( control ) {
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
			wolf_service_report_status( SERVICE_STOP_PENDING, NO_ERROR, 1000 );
			SetEvent( service_stop_event );
			break;

		case SERVICE_CONTROL_INTERROGATE:
			/* fall through to send current status */
			break;
	}

	/* report current state */
	wolf_service_report_status( service_status.dwCurrentState, NO_ERROR, 1000 );
}


	/* signal that we are now up and running */
	wolf_service_report_status( SERVICE_START_PENDING, NO_ERROR, 3000 );

	/* register a stop event, the service control handler will send
	 * the event. From now on we have a service event handler installed,
	 * so if something goes wrong we can set the service state to
	 * SERVICE_STOPPED and terminate gracefully.
	 */
	serviceStopEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	if( serviceStopEvent == NULL ) {
		
	if( service_stop_event == NULL ) {
		WOLF_LOG_GET_LAST_ERROR( GetLastError( ), errbuf, 512 );
		wolf_log( WOLF_LOG_ERR, WOLF_CATEGORY_SERVICE, WOLF_MSG_SERVICE_CANT_CREATE_STOP_EVENT,
			_( "Unable to create the stop event for service '%s': %s (%d)" ),
			SERVICE_NAME, errbuf, GetLastError( ) );
		wolf_service_report_status( SERVICE_STOPPED, NO_ERROR, 1000 );
	}

	wolf_service_report_status( SERVICE_RUNNING, NO_ERROR, 1000 );

	/* now call the user-defined service main function */
	service_service_main( argc, argv );

	wolf_service_report_status( SERVICE_STOPPED, NO_ERROR, 1000 );

	wolf_log_closelogtoeventlog( );

	return;
}

static void WINAPI service_main( DWORD argc, LPTSTR *argv ) {
	try {
// read configuration (from the location stored in the registry)
		_SMERP::CmdLineConfig cmdLineCfg; // empty for a service
		const char *configFile = _SMERP::CfgFileConfig::fileFromRegistry( );
		_SMERP::CfgFileConfig cfgFileCfg;
		if ( !cfgFileCfg.parse( configFile ))	{	// there was an error parsing the configuration file
			// TODO: a hen and egg problem here with event logging and where to know where to log to
			// LOG_FATAL << cmdLineCfg.errMsg();
			return;
		}
		_SMERP::ApplicationConfiguration config( cmdLineCfg, cfgFileCfg );

// Create the final logger based on the configuration
		_SMERP::Logger::initialize( config );

// register the event callback where we get called by Windows and the SCM
		serviceStatusHandle = RegisterServiceCtrlHandler( config.serviceName, serviceCtrlFunction );
		if( serviceStatusHandle == 0 ) {
			LOG_FATAL << "Unable to register service control handler function";
			return;
		}

// send "we are starting up now" to the SCM
		service_report_status( SERVICE_START_PENDING, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );

// register a stop event
		serviceStopEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		if( serviceStopEvent == NULL ) {
			LOG_FATAL( "Unable to create the stop event for the termination of the service" );
			service_report_status( SERVICE_STOPPED, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );
			return;
		}

		LOG_NOTICE << "Starting service";
		
// Run server in background thread(s).
		_SMERP::server s( config );
		boost::thread t( boost::bind( &_SMERP::server::run, &s ));

// we are up and running now (hopefully), signal this to the SCM
		service_report_status( SERVICE_RUNNING, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );

// Sit and wait here for the stop event to happen, terminate then


// signal the SCM that we are done
		service_report_status( SERVICE_STOPPED, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );

	wolf_log_closelogtoeventlog( );

	}
	catch (std::exception& e)	{
		LOG_FATAL << e.msg( );
// any fatal error should signal the SCM that the service is down now
		service_report_status( SERVICE_STOPPED, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );
	}
}

int _SMERP_winMain( int argc, char* argv[] )
{
	try	{
		_SMERP::AppInstance	app( MAJOR_VERSION, MINOR_VERSION, REVISION_NUMBER );
		_SMERP::CmdLineConfig	cmdLineCfg;
		const char		*configFile;

// TODO: service can't read command line options, read the only relevant one (the absolute
// path of the configuration file from the registry

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
			std::cout << "BOBOBO version " << app.version().toString() << std::endl << std::endl;
			return _SMERP::ErrorCodes::OK;
		}
		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::PRINT_HELP )	{
			cmdLineCfg.usage( std::cout );
			std::cerr << std::endl;
			return _SMERP::ErrorCodes::OK;
		}

// decide what configuration file to use
		if ( !cmdLineCfg.cfgFile.empty() )	// if it has been specified than that's The One ! (and only)
			configFile = cmdLineCfg.cfgFile.c_str();
		else
			configFile = _SMERP::CfgFileConfig::fileFromRegistry( );
		if ( configFile == NULL )	{	// there is no configuration file
			std::cerr << "MOMOMO: no configuration file found !" << std::endl << std::endl;
			return _SMERP::ErrorCodes::FAILURE;
		}

		_SMERP::CfgFileConfig	cfgFileCfg;
		if ( !cfgFileCfg.parse( configFile ))	{	// there was an error parsing the configuration file
			std::cerr << cfgFileCfg.errMsg() << std::endl << std::endl;
			return _SMERP::ErrorCodes::FAILURE;
		}
// configuration file has been parsed successfully
// build the application configuration
		_SMERP::ApplicationConfiguration config( cmdLineCfg, cfgFileCfg);

// Check the configuration
		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::CHECK_CONFIG )	{
			if ( config.check() )	{
				std::cout << "Configuration OK" << std::endl << std::endl;
				return _SMERP::ErrorCodes::OK;
			}
			else	{
				std::cout << config.errMsg() << std::endl << std::endl;
				return _SMERP::ErrorCodes::OK;
			}
		}

		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::PRINT_CONFIG )	{
			config.print( std::cout );
			std::cout << std::endl;
			return _SMERP::ErrorCodes::OK;
		}

		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::TEST_CONFIG )	{
			std::cout << "Not implemented yet" << std::endl << std::endl;
			return _SMERP::ErrorCodes::OK;
		}

		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::INSTALL_SERVICE ) {
			install_as_service( config );
			std::cout << "Installed as Windows service" << std::endl << std::endl;
			return _SMERP::ErrorCodes::OK;
		}
		
		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::REMOVE_SERVICE ) {
			remove_as_service( config );
			std::cout << "Removed as Windows service" << std::endl << std::endl;
			return _SMERP::ErrorCodes::OK;
		}
		
		// if started as service we dispatch the service thread now
		SERVICE_TABLE_ENTRY dispatch_table[2] =
			{ { const_cast<char *>( config.serviceName.c_str( ) ), service_main },
			{ NULL, NULL } };

		// go into service mode now eventually 
		if( !config.foreground ) {
			if( !StartServiceCtrlDispatcher( dispatch_table ) ) {
				if( GetLastError( ) == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT ) {
					// not called as service, continue as console application
				} else {
					return _SMERP::ErrorCodes::FAILURE;
				}
			} else {
				// here we get if the service has been stopped, so we terminate here
				return _SMERP::ErrorCodes::OK;
			}
		}
	
		// Create the final logger based on the configuration
		_SMERP::Logger::initialize( config );
		LOG_NOTICE << "Starting server";

		_SMERP::server s( config );

		// Set console control handler to allow server to be stopped.
		consoleCtrlFunction = boost::bind(&_SMERP::server::stop, &s);
		SetConsoleCtrlHandler(consoleCtrlHandler, TRUE);

		// Run the server until stopped.
		s.run();
	}
	catch (std::exception& e)	{
		std::cerr << "exception: " << e.what() << "\n";
		return _SMERP::ErrorCodes::FAILURE;
	}
	LOG_NOTICE << "Server stopped";

	return _SMERP::ErrorCodes::OK;
}

#endif // defined(_WIN32)
