//
// testing the boost logging library
//

#include "logger.hpp"

_SMERP::LogBackend logBack;

int main( void ) {
	logBack.setConsoleLevel( _SMERP::LogLevel::LOGLEVEL_DATA );
	logBack.setLogfileLevel( _SMERP::LogLevel::LOGLEVEL_DATA );
	logBack.setLogfileName( "logTest.log" );
#ifndef _WIN32
	logBack.setSyslogLevel( _SMERP::LogLevel::LOGLEVEL_DATA );
	logBack.setSyslogFacility( _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_USER );
	logBack.setSyslogIdent( "test" );
#else
	logBack.setEventlogLevel( _SMERP::LogLevel::LOGLEVEL_DATA );
	logBack.setEventlogSource( "smerptest" );
	logBack.setEventlogLog( "Application" );
#endif

	LOG_FATAL	<< "fatal error";
	LOG_ALERT	<< "alert";
	LOG_CRITICAL	<< "critical error";
	LOG_SEVERE	<< "severe error";
	LOG_ERROR	<< "an error";
	LOG_WARNING	<< "a warning";
	LOG_NOTICE	<< "a notice";
	LOG_INFO 	<< "an info";
	LOG_DEBUG	<< "debug message";
	LOG_TRACE	<< "debug message with tracing";
	LOG_DATA	<< "debug message with tracing and data";
	
	return 0;
}
