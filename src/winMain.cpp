//
// winMain.cpp
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/function.hpp>
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

#include "connectionHandler.hpp"

#if !defined(_WIN32)
#error "This is the WIN32 main !"
#else

#include <WinSvc.h>

#include <cstdio>
#include <sstream>


static const int DEFAULT_SERVICE_TIMEOUT = 5000;


// DUMMY
namespace _Wolframe	{
	struct HandlerConfiguration	{
	};
}

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
static void registerEventlog( const _Wolframe::Configuration::ApplicationConfiguration& config )
{
	char key[256];
	HKEY h = 0;
	DWORD disposition;

// choose the key for the EventLog registry entry
	_snprintf( key, 256, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s",
		config.logConfig->eventlogLogName.c_str( ), config.logConfig->eventlogSource.c_str( ) );
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

// exactly one category for now (Wolframe)
	registrySetWord( h, "CategoryCount", (DWORD)1 );

	(void)RegCloseKey( h );
}

static void deregisterEventlog( const _Wolframe::Configuration::ApplicationConfiguration& config )
{
	char key[256];
	HKEY h = 0;
	DWORD disposition;
	LONG res;

	_snprintf( key, 256, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s",
		config.logConfig->eventlogLogName.c_str( ) );
	res = RegOpenKeyEx( HKEY_LOCAL_MACHINE, key, 0, KEY_WRITE, &h );
	(void)RegDeleteKey( h, config.logConfig->eventlogSource.c_str( ) );
}

static void installAsService( const _Wolframe::Configuration::ApplicationConfiguration& config )
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
		config.srvConfig->serviceName.c_str( ), config.srvConfig->serviceDisplayName.c_str( ),
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		os.str( ).c_str( ), NULL, NULL, NULL, NULL, NULL );

// set description of the service
	SERVICE_DESCRIPTION descr;
	descr.lpDescription = (LPTSTR)config.srvConfig->serviceDescription.c_str( );
	(void)ChangeServiceConfig2( service, SERVICE_CONFIG_DESCRIPTION, &descr );

// free handles
	(void)CloseServiceHandle( service );
	(void)CloseServiceHandle( scm );
}

static void remove_as_service( const _Wolframe::Configuration::ApplicationConfiguration& config )
{
// get service control manager
	SC_HANDLE scm = (SC_HANDLE)OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS );

// get service handle of the service to delete (identified by service name)
	SC_HANDLE service = OpenService( scm, config.srvConfig->serviceName.c_str( ), SERVICE_ALL_ACCESS );

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
		_Wolframe::Configuration::CmdLineConfig cmdLineCfg; // empty for a service with --service
		cmdLineCfg.command = _Wolframe::Configuration::CmdLineConfig::RUN_SERVICE;
		const char *configFile = serviceConfig.c_str( ); // configuration comes from main thread

		std::stringstream	errMsg;
		_Wolframe::Configuration::ApplicationConfiguration config;
		if ( !config.parse( configFile, errMsg ))	{	// there was an error parsing the configuration file
			// TODO: a hen and egg problem here with event logging and where to know where to log to
			// LOG_FATAL << errMsg.str();
			return;
		}
// configuration file has been parsed successfully
// build the final configuration
		config.finalize( cmdLineCfg );

// create the final logger based on the configuration
		_Wolframe::LogBackend::instance().setLogfileLevel( config.logConfig->logFileLogLevel );
		_Wolframe::LogBackend::instance().setLogfileName( config.logConfig->logFile );
		_Wolframe::LogBackend::instance().setEventlogLevel( config.logConfig->eventlogLogLevel );
		_Wolframe::LogBackend::instance().setEventlogSource( config.logConfig->eventlogSource );
		_Wolframe::LogBackend::instance().setEventlogLog( config.logConfig->eventlogLogName );

// register the event callback where we get called by Windows and the SCM
		serviceStatusHandle = RegisterServiceCtrlHandler( config.srvConfig->serviceName.c_str( ), serviceCtrlFunction );
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
		_Wolframe::ServerHandler	handler( config.handlerConfig );
		_Wolframe::Network::server s( config.srvConfig->address, config.srvConfig->SSLaddress,
								handler, config.srvConfig->threads, config.srvConfig->maxConnections );
		boost::thread t( boost::bind( &_Wolframe::Network::server::run, &s ));

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
				return;
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


int _Wolframe_winMain( int argc, char* argv[] )
{
	try	{
		_Wolframe::Version		appVersion( _Wolframe::applicationMajorVersion(), _Wolframe::applicationMinorVersion(),
										_Wolframe::applicationRevisionVersion(), _Wolframe::applicationBuildVersion() );
		_Wolframe::Configuration::CmdLineConfig	cmdLineCfg;
		const char		*configFile = NULL;

// it's just a DUMMY for now
		_Wolframe::HandlerConfiguration	handlerConfig;

		if ( !cmdLineCfg.parse( argc, argv ))	{	// there was an error parsing the command line
			std::cerr << cmdLineCfg.errMsg() << std::endl << std::endl;
			cmdLineCfg.usage( std::cerr );
			std::cerr << std::endl;
			return _Wolframe::ErrorCodes::FAILURE;
		}
// command line has been parsed successfully
// if cmdLineCfg.errMsg() is not empty than we have a warning
		if ( !cmdLineCfg.errMsg().empty() )	// there was a warning parsing the command line
			std::cerr << "BOO:" << cmdLineCfg.errMsg() << std::endl << std::endl;

// if we have to print the version or the help do it and exit
		if ( cmdLineCfg.command == _Wolframe::Configuration::CmdLineConfig::PRINT_VERSION )	{
			std::cout << "BOBOBO version " << appVersion.toString() << std::endl << std::endl;
			return _Wolframe::ErrorCodes::OK;
		}
		if ( cmdLineCfg.command == _Wolframe::Configuration::CmdLineConfig::PRINT_HELP )	{
			cmdLineCfg.usage( std::cout );
			std::cerr << std::endl;
			return _Wolframe::ErrorCodes::OK;
		}

// decide what configuration file to use
		if ( !cmdLineCfg.cfgFile.empty() )	// if it has been specified than that's The One ! (and only)
			configFile = cmdLineCfg.cfgFile.c_str();
		if ( configFile == NULL )	{	// there is no configuration file
			std::cerr << "MOMOMO: no configuration file found !" << std::endl << std::endl;
			return _Wolframe::ErrorCodes::FAILURE;
		}

		_Wolframe::Configuration::ApplicationConfiguration config;
		std::stringstream	errMsg;
		if ( !config.parse( configFile, errMsg ))	{	// there was an error parsing the configuration file
			std::cerr << errMsg.str() << std::endl << std::endl;
			return _Wolframe::ErrorCodes::FAILURE;
		}
// configuration file has been parsed successfully
// build the final configuration
		config.finalize( cmdLineCfg );

// Check the configuration
		if ( cmdLineCfg.command == _Wolframe::Configuration::CmdLineConfig::CHECK_CONFIG )	{
			if ( config.check( errMsg ) )	{
				std::cout << "Configuration OK" << std::endl << std::endl;
				return _Wolframe::ErrorCodes::OK;
			}
			else	{
				std::cout << errMsg.str() << std::endl << std::endl;
				return _Wolframe::ErrorCodes::OK;
			}
		}

		if ( cmdLineCfg.command == _Wolframe::Configuration::CmdLineConfig::PRINT_CONFIG )	{
			config.print( std::cout );
			std::cout << std::endl;
			return _Wolframe::ErrorCodes::OK;
		}

		if ( cmdLineCfg.command == _Wolframe::Configuration::CmdLineConfig::TEST_CONFIG )	{
			std::cout << "Not implemented yet" << std::endl << std::endl;
			return _Wolframe::ErrorCodes::OK;
		}

		if ( cmdLineCfg.command == _Wolframe::Configuration::CmdLineConfig::INSTALL_SERVICE ) {
			registerEventlog( config );
			installAsService( config );
			std::cout << "Installed as Windows service" << std::endl << std::endl;
			return _Wolframe::ErrorCodes::OK;
		}

		if ( cmdLineCfg.command == _Wolframe::Configuration::CmdLineConfig::REMOVE_SERVICE ) {
			remove_as_service( config );
			deregisterEventlog( config );
			std::cout << "Removed as Windows service" << std::endl << std::endl;
			return _Wolframe::ErrorCodes::OK;
		}

		if( cmdLineCfg.command == _Wolframe::Configuration::CmdLineConfig::RUN_SERVICE ) {
			// if started as service we dispatch the service thread now
			SERVICE_TABLE_ENTRY dispatch_table[2] =
				{ { const_cast<char *>( config.srvConfig->serviceName.c_str( ) ), service_main },
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
					return _Wolframe::ErrorCodes::FAILURE;
				}
			} else {
				// here we get if the service has been stopped, so we terminate here
				return _Wolframe::ErrorCodes::OK;
			}
		}

		// Create the final logger based on the configuration, this is the
		// foreground mode in a console, so we start only the stderr logger
		_Wolframe::LogBackend::instance().setConsoleLevel( config.logConfig->stderrLogLevel );

		LOG_NOTICE << "Starting server";

		_Wolframe::ServerHandler	handler( config.handlerConfig );
		_Wolframe::Network::server s( config.srvConfig->address, config.srvConfig->SSLaddress,
					   handler, config.srvConfig->threads, config.srvConfig->maxConnections );

		// Set console control handler to allow server to be stopped.
		consoleCtrlFunction = boost::bind(&_Wolframe::Network::server::stop, &s);
		SetConsoleCtrlHandler(consoleCtrlHandler, TRUE);

		// Run the server until stopped.
		s.run();
	}
	catch (std::exception& e)	{
		std::cerr << "exception: " << e.what() << "\n";
		return _Wolframe::ErrorCodes::FAILURE;
	}
	LOG_NOTICE << "Server stopped";

	return _Wolframe::ErrorCodes::OK;
}

#endif // defined(_WIN32)
