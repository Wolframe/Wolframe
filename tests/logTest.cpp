//
// testing the Logging library
//

#include "logger.hpp"
#include <gtest/gtest.h>

#ifndef _WIN32
#include <sys/stat.h>
#include <fcntl.h>
#else
#include <windows.h>
#endif

using namespace _Wolframe::Logging;

// The fixture for testing class _Wolframe::Logging
class LoggingFixture : public ::testing::Test
{
	private:
		LogBackend& logBack;
		
	protected:
		LoggingFixture( ) :
			logBack( LogBackend::instance( ) ) 
		{
			// in order not to spoil the output of gtest :-)
			logBack.setConsoleLevel( LogLevel::LOGLEVEL_UNDEFINED );

			logBack.setLogfileLevel( LogLevel::LOGLEVEL_DATA );
			logBack.setLogfileName( "logTest.log" );
#ifndef _WIN32
			logBack.setSyslogLevel( LogLevel::LOGLEVEL_DATA );
			logBack.setSyslogFacility( SyslogFacility::WOLFRAME_SYSLOG_FACILITY_USER );
			logBack.setSyslogIdent( "logTest" );
#else
			logBack.setEventlogLevel( LogLevel::LOGLEVEL_DATA );
			logBack.setEventlogSource( "logTest" );
			logBack.setEventlogLog( "Application" );
#endif
		}
};

TEST_F( LoggingFixture, LogMacrosWithoutComponent )
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

TEST_F( LoggingFixture, LogMacrosWithComponent )
{
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

#ifndef _WIN32
TEST_F( LoggingFixture, LogSystemErrorMarkersUnix )
{
	(void)open( "bla", O_RDONLY, 0 );
	LOG_ERROR << "open failed, reason: " << LogStrerror;
}
#endif

#ifdef _WIN32
TEST_F( LoggingFixture, LogSystemErrorMarkersWin )
{
	OFSTRUCT s;
	HFILE h = OpenFile( "bla", &s, OF_READ );
	if( h == HFILE_ERROR ) {
		LOG_ERROR << "OpenFile failed, reason: " << LogWinerror;
	}
}
#endif

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
