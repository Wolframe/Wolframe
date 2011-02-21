//
// testing the boost logging library
//

#include "logger.hpp"
#include <gtest/gtest.h>

// The fixture for testing class SMERP::Version
class LoggingFixture : public ::testing::Test
{
	private:
		_SMERP::LogBackend& logBack;
		
	protected:
		LoggingFixture( ) :
			logBack( _SMERP::LogBackend::instance( ) ) 
		{
			// in order not to spoil the output of gtest :-)
			logBack.setConsoleLevel( _SMERP::LogLevel::LOGLEVEL_UNDEFINED );
			logBack.setConsolePrefix( "smerp" );

			logBack.setLogfileLevel( _SMERP::LogLevel::LOGLEVEL_DATA );
			logBack.setLogfileName( "logTest.log" );
			logBack.setLogfilePrefix( "smerp" );
#ifndef _WIN32
			logBack.setSyslogLevel( _SMERP::LogLevel::LOGLEVEL_DATA );
			logBack.setSyslogFacility( _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_USER );
			logBack.setSyslogIdent( "smerptest" );
#else
			logBack.setEventlogLevel( _SMERP::LogLevel::LOGLEVEL_DATA );
			logBack.setEventlogSource( "smerptest" );
			logBack.setEventlogLog( "Application" );
#endif
		}
};

TEST_F( LoggingFixture, Macros )
{
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
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
