//
// winMain.cpp
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "version.hpp"
#include "commandLine.hpp"
#include "configFile.hpp"
#include "appConfig.hpp"
#include "server.hpp"
#include "ErrorCodes.hpp"
#include "logger.hpp"

#include "smerpHandler.hpp"

#if !defined(_WIN32)
#error "This is the WIN32 main !"
#else

#include <WinSvc.h>

#include <stdio.h>

static const unsigned short MAJOR_VERSION = 0;
static const short unsigned MINOR_VERSION = 0;
static const short unsigned REVISION_NUMBER = 3;

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

static void registrySetString( HKEY h, TCHAR *name, TCHAR *value ) {
	(void)RegSetValueEx( h, name, 0, REG_EXPAND_SZ, (LPBYTE)value, strlen( value ) );
}

static void registrySetWord( HKEY h, TCHAR *name, DWORD value ) {
	(void)RegSetValueEx( h, name, 0, REG_DWORD, (LPBYTE)&value, sizeof( DWORD ) );
}

// initializes the Event Logger
static void registerEventlog( const _SMERP::ApplicationConfiguration& config )
{
	char key[256];
	HKEY h = 0;
	DWORD disposition;

// choose the key for the EventLog registry entry
	_snprintf( key, 256, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s",
		config.eventlogLogName.c_str( ), config.eventlogSource.c_str( ) );
	(void)RegCreateKeyEx( HKEY_LOCAL_MACHINE, key, 0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_SET_VALUE, NULL, &h, &disposition );

// retrieve absolute path of binary
	TCHAR binary_path[MAX_PATH];
	DWORD res = GetModuleFileName( NULL, binary_path, MAX_PATH );

// register resources in the service binary itself as message source
	registrySetString( h, "EventMessageFile", binary_path );
	registrySetString( h, "CategoryMessageFile", binary_path );

// supported event types
	DWORD eventTypes = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
	registrySetWord( h, "TypesSupported", eventTypes );

// exactly one category for now (smerp)
	registrySetWord( h, "CategoryCount", (DWORD)1 );

	(void)RegCloseKey( h );
}

static void deregisterEventlog( const _SMERP::ApplicationConfiguration& config )
{
	char key[256];
	HKEY h = 0;
	DWORD disposition;
	LONG res;

	_snprintf( key, 256, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s",
		config.eventlogLogName.c_str( ) );
	res = RegOpenKeyEx( HKEY_LOCAL_MACHINE, key, 0, KEY_WRITE, &h );
	(void)RegDeleteKey( h, config.eventlogSource.c_str( ) );
}

static void installAsService( const _SMERP::ApplicationConfiguration& config )
{
// get service control manager
	SC_HANDLE scm = (SC_HANDLE)OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS );

// retrieve absolute path of binary
	TCHAR binary_path[MAX_PATH];
	DWORD res = GetModuleFileName( NULL, binary_path, MAX_PATH );

// add quotation marks around filename in the 'ImagePath' (because of spaces).
	std::ostringstream os;
	os << "\"" << binary_path << "\" --service -c \"" << config.configFile << "\"";

// create the service
	SC_HANDLE service = CreateService( scm,
		config.serviceName.c_str( ), config.serviceDisplayName.c_str( ),
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		os.str( ).c_str( ), NULL, NULL, NULL, NULL, NULL );

// set description of the service
	SERVICE_DESCRIPTION descr;
	descr.lpDescription = (LPTSTR)config.serviceDescription.c_str( );
	(void)ChangeServiceConfig2( service, SERVICE_CONFIG_DESCRIPTION, &descr );

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

// free handles
	(void)CloseServiceHandle( service );
	(void)CloseServiceHandle( scm );
}

// state and helper variables for the service
static SERVICE_STATUS_HANDLE serviceStatusHandle;
static HANDLE serviceStopEvent = NULL;
static SERVICE_STATUS serviceStatus;

// helper function to report
static void service_report_status(	DWORD currentState,
					DWORD exitCode,
					DWORD waitHint )
{
	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = currentState;
	serviceStatus.dwWin32ExitCode = exitCode;
	serviceStatus.dwServiceSpecificExitCode = 0;
	serviceStatus.dwWaitHint = waitHint;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;

	switch( currentState ) {
		case SERVICE_START_PENDING:
// during startup we should not accept other events
			serviceStatus.dwControlsAccepted = 0;
			serviceStatus.dwCheckPoint++;
			break;

		case SERVICE_RUNNING:
		case SERVICE_STOPPED:
// reset checkpoint for drawing the progress bar in GUIs
			serviceStatus.dwCheckPoint = 0;
			break;

		default:
// increase the checkpoint ticks for the progress bar in GUIs
			serviceStatus.dwCheckPoint++;
			break;
	}

	if( !SetServiceStatus( serviceStatusHandle, &serviceStatus ) ) {
		LOG_FATAL << "Unable to report service state " << currentState << " to SCM";
		return;
	}
}

// events from SCM and Windows comes here. Be responsive!!
void WINAPI serviceCtrlFunction( DWORD control )
{
	switch( control ) {
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
			service_report_status( SERVICE_STOP_PENDING, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );
			LOG_INFO << "Stopping service";
// signal stopping now
			SetEvent( serviceStopEvent );
			break;

		case SERVICE_CONTROL_INTERROGATE:
			// see below
			break;
	}

// report current status of service to SCM
	service_report_status( serviceStatus.dwCurrentState, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );
}

// for passing the location of the configuration
static std::string serviceConfig;

static void WINAPI service_main( DWORD argc, LPTSTR *argv ) {
	try {
// read configuration (from the location passed in the command line arguments of the main, not the service_main)
		_SMERP::CmdLineConfig cmdLineCfg; // empty for a service with --service
		cmdLineCfg.command = _SMERP::CmdLineConfig::RUN_SERVICE;
		const char *configFile = serviceConfig.c_str( ); // configuration comes from main thread
		_SMERP::CfgFileConfig cfgFileCfg;
		if ( !cfgFileCfg.parse( configFile ))	{	// there was an error parsing the configuration file
			// TODO: a hen and egg problem here with event logging and where to know where to log to
			// LOG_FATAL << cmdLineCfg.errMsg();
			return;
		}
		_SMERP::ApplicationConfiguration config( cmdLineCfg, cfgFileCfg );

// create the final logger based on the configuration
		logBack.setLogfileLevel( config.logFileLogLevel );
		logBack.setLogfileName( config.logFile );
		logBack.setEventlogLevel( config.eventlogLogLevel );
		logBack.setEventlogSource( config.eventlogSource );
		logBack.setEventlogLog( config.eventlogLogName );

// register the event callback where we get called by Windows and the SCM
		serviceStatusHandle = RegisterServiceCtrlHandler( config.serviceName.c_str( ), serviceCtrlFunction );
		if( serviceStatusHandle == 0 ) {
			LOG_FATAL << "Unable to register service control handler function";
			return;
		}

// send "we are starting up now" to the SCM
		service_report_status( SERVICE_START_PENDING, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );

// register a stop event
		serviceStopEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		if( serviceStopEvent == NULL ) {
			LOG_FATAL << "Unable to create the stop event for the termination of the service";
			service_report_status( SERVICE_STOPPED, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );
			return;
		}

		LOG_NOTICE << "Starting service";

// run server in background thread(s).
		_SMERP::echoServer	echo;
		_SMERP::Network::server s( config.address, config.SSLaddress, echo, config.threads, config.maxConnections );
		boost::thread t( boost::bind( &_SMERP::Network::server::run, &s ));

// we are up and running now (hopefully), signal this to the SCM
		service_report_status( SERVICE_RUNNING, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );

// Sit and wait here for the stop event to happen, terminate then
WAIT_FOR_STOP_EVENT:
		DWORD res = WaitForSingleObject( serviceStopEvent, DEFAULT_SERVICE_TIMEOUT );
		switch( res ) {
			case WAIT_OBJECT_0:
				s.stop( );
// stop signal received, signal "going to stop" to SCM
				service_report_status( SERVICE_STOP_PENDING, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );
				break;

			case WAIT_TIMEOUT:
// we could do something periodic here
				goto WAIT_FOR_STOP_EVENT;

			default:
// error, stop now immediatelly
				LOG_FATAL << "Waiting for stop event in service main failed, stopping now";
				s.stop( );
				service_report_status( SERVICE_STOPPED, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );
		}

// signal the SCM that we are done
		LOG_NOTICE << "Stopped service";
		service_report_status( SERVICE_STOPPED, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );
	}
	catch (std::exception& e)	{
		LOG_FATAL << e.what( );
// any exception should signal the SCM that the service is down now
		service_report_status( SERVICE_STOPPED, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );
	}
}


_SMERP::LogBackend	logBack;

int _SMERP_winMain( int argc, char* argv[] )
{
	try	{
		_SMERP::Version		appVersion( MAJOR_VERSION, MINOR_VERSION, REVISION_NUMBER );
		_SMERP::CmdLineConfig	cmdLineCfg;
		const char		*configFile = NULL;

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
			std::cout << "BOBOBO version " << appVersion.toString() << std::endl << std::endl;
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
			registerEventlog( config );
			installAsService( config );
			std::cout << "Installed as Windows service" << std::endl << std::endl;
			return _SMERP::ErrorCodes::OK;
		}

		if ( cmdLineCfg.command == _SMERP::CmdLineConfig::REMOVE_SERVICE ) {
			remove_as_service( config );
			deregisterEventlog( config );
			std::cout << "Removed as Windows service" << std::endl << std::endl;
			return _SMERP::ErrorCodes::OK;
		}

		if( cmdLineCfg.command == _SMERP::CmdLineConfig::RUN_SERVICE ) {
			// if started as service we dispatch the service thread now
			SERVICE_TABLE_ENTRY dispatch_table[2] =
				{ { const_cast<char *>( config.serviceName.c_str( ) ), service_main },
				{ NULL, NULL } };

			// pass configuration to service main
			serviceConfig = config.configFile;

			if( !StartServiceCtrlDispatcher( dispatch_table ) ) {
				if( GetLastError( ) == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT ) {
					// not called as service, continue as console application
					config.foreground = true;
				} else {
					// TODO: mmh? what are we doing here? No longer here
					LOG_FATAL << "Unable to dispatch service control dispatcher";
					return _SMERP::ErrorCodes::FAILURE;
				}
			} else {
				// here we get if the service has been stopped, so we terminate here
				return _SMERP::ErrorCodes::OK;
			}
		}

		// Create the final logger based on the configuration, this is the
		// foreground mode in a console, so we start only the stderr logger
		logBack.setConsoleLevel( config.stderrLogLevel );

		LOG_NOTICE << "Starting server";

		_SMERP::echoServer	echo;
		_SMERP::Network::server s( config.address, config.SSLaddress,
					   echo, config.threads, config.maxConnections );

		// Set console control handler to allow server to be stopped.
		consoleCtrlFunction = boost::bind(&_SMERP::Network::server::stop, &s);
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
