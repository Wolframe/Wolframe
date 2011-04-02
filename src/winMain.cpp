//
// winMain.cpp
//

#include <iostream>
#include <string>
#include <TCHAR.h>
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
	(void)RegSetValueEx( h, name, 0, REG_EXPAND_SZ, (LPBYTE)value, (DWORD)_tcslen( value ) );
}

static void registrySetWord( HKEY h, TCHAR *name, DWORD value ) {
	(void)RegSetValueEx( h, name, 0, REG_DWORD, (LPBYTE)&value, sizeof( DWORD ) );
}

// initializes the Event Logger
static bool registerEventlog( const _Wolframe::Configuration::ApplicationConfiguration& config )
{
	char key[256];
	HKEY h = 0;
	DWORD disposition;
	
// choose the key for the EventLog registry entry
	_snprintf( key, 256, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s",
		config.loggerConf->eventlogLogName.c_str( ), config.loggerConf->eventlogSource.c_str( ) );
	LONG ret = RegCreateKeyEx( HKEY_LOCAL_MACHINE, key, 0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_SET_VALUE, NULL, &h, &disposition );
	if( ret != ERROR_SUCCESS ) {
		LOG_CRITICAL << "RegCreateKeyEx with key '" << key << "' failed: " << _Wolframe::Logging::LogError::LogWinerror;
		return false;
	}

// retrieve absolute path of binary
	TCHAR binary_path[MAX_PATH];
	DWORD res = GetModuleFileName( NULL, binary_path, MAX_PATH );

// register resources in the service binary itself as message source
	registrySetString( h, TEXT( "EventMessageFile" ), binary_path );
	registrySetString( h, TEXT( "CategoryMessageFile" ), binary_path );

// supported event types
	DWORD eventTypes = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
	registrySetWord( h, TEXT( "TypesSupported" ), eventTypes );

// must be in sync with LogComponent in logger.hpp and 'logComponentToCategoryId' in logBackend.cpp
	registrySetWord( h, TEXT( "CategoryCount" ), (DWORD)5 );

	(void)RegCloseKey( h );
	
	return true;
}

static bool deregisterEventlog( const _Wolframe::Configuration::ApplicationConfiguration& config )
{
	char key[256];
	HKEY h = 0;
	DWORD disposition;
	LONG res;

// remove event log registry entry	
	_snprintf( key, 256, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s",
		config.loggerConf->eventlogLogName.c_str( ), config.loggerConf->eventlogSource.c_str( ) );
	res = RegDeleteKey( HKEY_LOCAL_MACHINE, key );
	if( res != ERROR_SUCCESS ) {
		LOG_CRITICAL << "RegDeleteKey with key '" << key << "' failed: " << _Wolframe::Logging::LogError::LogWinerror;
		return false;
	}
	
	return true;
}

static bool installAsService( const _Wolframe::Configuration::ApplicationConfiguration& config )
{
// get service control manager
	SC_HANDLE scm = (SC_HANDLE)OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS );
	if( scm == NULL ) {
		LOG_CRITICAL << "OpenSCManager for service registration failed: " << _Wolframe::Logging::LogError::LogWinerror;
		return false;
	}
	
// retrieve absolute path of binary
	TCHAR binary_path[MAX_PATH];
	DWORD res = GetModuleFileName( NULL, binary_path, MAX_PATH );

// add quotation marks around filename in the 'ImagePath' (because of spaces).
	std::ostringstream os;
	os << "\"" << binary_path << "\" --service -c \"" << config.configFile << "\"";

// create the service
	SC_HANDLE service = CreateService( scm,
		config.serviceConf->serviceName.c_str( ), config.serviceConf->serviceDisplayName.c_str( ),
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		os.str( ).c_str( ), NULL, NULL, NULL, NULL, NULL );
	if( service == NULL ) {
		LOG_CRITICAL << "CreateService during service registration failed: " << _Wolframe::Logging::LogError::LogWinerror;
		return false;
	}

// set description of the service
	SERVICE_DESCRIPTION descr;
	descr.lpDescription = (LPTSTR)config.serviceConf->serviceDescription.c_str( );
	(void)ChangeServiceConfig2( service, SERVICE_CONFIG_DESCRIPTION, &descr );

// free handles
	(void)CloseServiceHandle( service );
	(void)CloseServiceHandle( scm );
	
	return true;
}

static bool removeAsService( const _Wolframe::Configuration::ApplicationConfiguration& config )
{
// get service control manager
	SC_HANDLE scm = (SC_HANDLE)OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS );
	if( scm == NULL ) {
		LOG_CRITICAL << "OpenSCManager for service deregistration failed: " << _Wolframe::Logging::LogError::LogWinerror;
		return false;
	}

// get service handle of the service to delete (identified by service name)
	SC_HANDLE service = OpenService( scm, config.serviceConf->serviceName.c_str( ), SERVICE_ALL_ACCESS );
	if( service == NULL ) {
		LOG_CRITICAL << "OpenService during service deregistration failed: " << _Wolframe::Logging::LogError::LogWinerror;
		return false;
	}

// remove the service
	if( !DeleteService( service ) ) {
		LOG_CRITICAL << "Can't delete service: " << _Wolframe::Logging::LogError::LogWinerror;
		return false;
	}

// free handles
	(void)CloseServiceHandle( service );
	(void)CloseServiceHandle( scm );
	
	return true;
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
		LOG_FATAL << "Unable to report service state " << currentState << " to SCM: " << _Wolframe::Logging::LogError::LogWinerror;
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

		_Wolframe::Configuration::ApplicationConfiguration config;
		if ( !config.parse( configFile ))	// there was an error parsing the configuration file
			return;
// configuration file has been parsed successfully
// build the final configuration
		config.finalize( cmdLineCfg );

// create the final logger based on the configuration
		_Wolframe::Logging::LogBackend::instance().setConsoleLevel( config.loggerConf->stderrLogLevel );
		_Wolframe::Logging::LogBackend::instance().setLogfileLevel( config.loggerConf->logFileLogLevel );
		_Wolframe::Logging::LogBackend::instance().setLogfileName( config.loggerConf->logFile );
		_Wolframe::Logging::LogBackend::instance().setSyslogLevel( config.loggerConf->syslogLogLevel );
		_Wolframe::Logging::LogBackend::instance().setSyslogFacility( config.loggerConf->syslogFacility );
		_Wolframe::Logging::LogBackend::instance().setSyslogIdent( config.loggerConf->syslogIdent );
		_Wolframe::Logging::LogBackend::instance().setEventlogLevel( config.loggerConf->eventlogLogLevel );
		_Wolframe::Logging::LogBackend::instance().setEventlogSource( config.loggerConf->eventlogSource );
		_Wolframe::Logging::LogBackend::instance().setEventlogLog( config.loggerConf->eventlogLogName );

// register the event callback where we get called by Windows and the SCM
		serviceStatusHandle = RegisterServiceCtrlHandler( config.serviceConf->serviceName.c_str( ), serviceCtrlFunction );
		if( serviceStatusHandle == 0 ) {
			LOG_FATAL << "Unable to register service control handler function: " << _Wolframe::Logging::LogError::LogWinerror;
			return;
		}

// send "we are starting up now" to the SCM
		service_report_status( SERVICE_START_PENDING, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );

// register a stop event
		serviceStopEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		if( serviceStopEvent == NULL ) {
			LOG_FATAL << "Unable to create the stop event for the termination of the service: " << _Wolframe::Logging::LogError::LogWinerror;
			service_report_status( SERVICE_STOPPED, NO_ERROR, DEFAULT_SERVICE_TIMEOUT );
			return;
		}

		LOG_NOTICE << "Starting service";

// run server in background thread(s).
		_Wolframe::ServerHandler handler( config.handlerConf );
		_Wolframe::Network::server s( config.serverConf, handler );
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

			case WAIT_ABANDONED:
// this is bad, not really sure how we could recover from this one. For
// now let's assume that stopping here is safer, so treat it like a successful
// event, but log the fact we run into that state
				LOG_CRITICAL << "Waiting for stop event in service main resulted in WAIT_ABANDONED!";
				break;
				
			case WAIT_FAILED:
// error, stop now immediatelly
				LOG_FATAL << "Waiting for stop event in service main failed, stopping now" << _Wolframe::Logging::LogError::LogWinerror;
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
		// create initial console logger, so we see things going wrong
		_Wolframe::Logging::LogBackend::instance().setConsoleLevel( _Wolframe::Logging::LogLevel::LOGLEVEL_INFO );
		
		_Wolframe::Version  appVersion( _Wolframe::applicationMajorVersion(), _Wolframe::applicationMinorVersion(),
						_Wolframe::applicationRevisionVersion(), _Wolframe::applicationBuildVersion() );
		_Wolframe::Configuration::CmdLineConfig	cmdLineCfg;
		const char		*configFile = NULL;

		if ( !cmdLineCfg.parse( argc, argv ))	{	// there was an error parsing the command line
			LOG_ERROR << cmdLineCfg.errMsg();
			cmdLineCfg.usage( std::cerr );
			std::cerr << std::endl;
			return _Wolframe::ErrorCodes::FAILURE;
		}
// command line has been parsed successfully
// if cmdLineCfg.errMsg() is not empty than we have a warning
		if ( !cmdLineCfg.errMsg().empty() )	// there was a warning parsing the command line
			LOG_ERROR << cmdLineCfg.errMsg();

// if we have to print the version or the help do it and exit
		if ( cmdLineCfg.command == _Wolframe::Configuration::CmdLineConfig::PRINT_VERSION )	{
			std::cout << "BOBOBO version " << appVersion.toString() << std::endl << std::endl;
			return _Wolframe::ErrorCodes::OK;
		}
		if ( cmdLineCfg.command == _Wolframe::Configuration::CmdLineConfig::PRINT_HELP )	{
			cmdLineCfg.usage( std::cout );
			std::cout << std::endl;
			return _Wolframe::ErrorCodes::OK;
		}

// decide what configuration file to use
		if ( !cmdLineCfg.cfgFile.empty() )	// if it has been specified than that's The One ! (and only)
			configFile = cmdLineCfg.cfgFile.c_str();
		if ( configFile == NULL )	{	// there is no configuration file
			LOG_FATAL << "no configuration file found !";
			return _Wolframe::ErrorCodes::FAILURE;
		}

		_Wolframe::Configuration::ApplicationConfiguration config;
		if ( !config.parse( configFile ))	// there was an error parsing the configuration file
			return _Wolframe::ErrorCodes::FAILURE;

// configuration file has been parsed successfully
// build the final configuration
		config.finalize( cmdLineCfg );

// Check the configuration
		if ( cmdLineCfg.command == _Wolframe::Configuration::CmdLineConfig::CHECK_CONFIG )	{
			if ( config.check() )	{
				std::cout << "Configuration OK" << std::endl << std::endl;
				return _Wolframe::ErrorCodes::OK;
			}
			else	{
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
			if( !registerEventlog( config ) ) return _Wolframe::ErrorCodes::FAILURE;
			if( !installAsService( config ) ) return _Wolframe::ErrorCodes::FAILURE;
			LOG_INFO << "Installed as Windows service '" << config.serviceConf->serviceName.c_str( ) << "'";
			return _Wolframe::ErrorCodes::OK;
		}

		if ( cmdLineCfg.command == _Wolframe::Configuration::CmdLineConfig::REMOVE_SERVICE ) {
			if( !removeAsService( config ) ) return _Wolframe::ErrorCodes::FAILURE;
			(void)deregisterEventlog( config );
			LOG_INFO << "Removed as Windows service '" << config.serviceConf->serviceName.c_str( ) << "'";
			return _Wolframe::ErrorCodes::OK;
		}

		if( cmdLineCfg.command == _Wolframe::Configuration::CmdLineConfig::RUN_SERVICE ) {
			// if started as service we dispatch the service thread now
			SERVICE_TABLE_ENTRY dispatch_table[2] =
				{ { const_cast<char *>( config.serviceConf->serviceName.c_str( ) ), service_main },
				{ NULL, NULL } };

			// pass configuration to service main
			serviceConfig = config.configFile;

			if( !StartServiceCtrlDispatcher( dispatch_table ) ) {
				if( GetLastError( ) == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT ) {
					// not called as service, continue as console application
					config.foreground = true;
				} else {
					// TODO: mmh? what are we doing here? No longer here
					LOG_FATAL << "Unable to dispatch service control dispatcher: " << _Wolframe::Logging::LogError::LogWinerror;
					return _Wolframe::ErrorCodes::FAILURE;
				}
			} else {
				// here we get if the service has been stopped, so we terminate here
				return _Wolframe::ErrorCodes::OK;
			}
		}

		// Create the final logger based on the configuration, this is the
		// foreground mode in a console, so we start only the stderr logger
		_Wolframe::Logging::LogBackend::instance().setConsoleLevel( config.loggerConf->stderrLogLevel );

		LOG_NOTICE << "Starting server";

		_Wolframe::ServerHandler handler( config.handlerConf );
		_Wolframe::Network::server s( config.serverConf, handler );

		// Set console control handler to allow server to be stopped.
		consoleCtrlFunction = boost::bind(&_Wolframe::Network::server::stop, &s);
		SetConsoleCtrlHandler(consoleCtrlHandler, TRUE);

		// Run the server until stopped.
		s.run();
	}
	catch (std::exception& e)	{
		LOG_ERROR << "Got exception: " << e.what( );
		return _Wolframe::ErrorCodes::FAILURE;
	}
	LOG_NOTICE << "Server stopped";

	return _Wolframe::ErrorCodes::OK;
}

#endif // defined(_WIN32)
