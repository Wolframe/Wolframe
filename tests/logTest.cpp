//
// testing the boost logging library
//

#include "logger.hpp"
#include <gtest/gtest.h>

// The fixture for testing class SMERP::Version
class LoggingFixture : public ::testing::Test
{
	private:
		_Wolframe::LogBackend& logBack;
		
	protected:
		LoggingFixture( ) :
			logBack( _Wolframe::LogBackend::instance( ) ) 
		{
			// in order not to spoil the output of gtest :-)
			logBack.setConsoleLevel( _Wolframe::LogLevel::LOGLEVEL_UNDEFINED );

			logBack.setLogfileLevel( _Wolframe::LogLevel::LOGLEVEL_DATA );
			logBack.setLogfileName( "logTest.log" );
#ifndef _WIN32
			logBack.setSyslogLevel( _Wolframe::LogLevel::LOGLEVEL_DATA );
			logBack.setSyslogFacility( _Wolframe::SyslogFacility::_Wolframe_SYSLOG_FACILITY_USER );
			logBack.setSyslogIdent( "smerptest" );
#else
			logBack.setEventlogLevel( _Wolframe::LogLevel::LOGLEVEL_DATA );
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

	LOG_NETWORK_FATAL	<< "fatal error";
	LOG_NETWORK_ALERT	<< "alert";
	LOG_NETWORK_CRITICAL	<< "critical error";
	LOG_NETWORK_SEVERE	<< "severe error";
	LOG_NETWORK_ERROR	<< "an error";
	LOG_NETWORK_WARNING	<< "a warning";
	LOG_NETWORK_NOTICE	<< "a notice";
	LOG_NETWORK_INFO 	<< "an info";
	LOG_NETWORK_DEBUG	<< "debug message";
	LOG_NETWORK_TRACE	<< "debug message with tracing";
	LOG_NETWORK_DATA	<< "debug message with tracing and data";
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
