//
// testing the Logging library
//

#include "logger.hpp"
#include <gtest/gtest.h>

#if !defined( _WIN32 )
#include <sys/stat.h>
#include <fcntl.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

using namespace _Wolframe::log;

// The fixture for testing class _Wolframe::log
class LoggingFixture : public ::testing::Test
{
	protected:
		LogBackend& logBack;
		
	protected:
		LoggingFixture( ) :
			logBack( LogBackend::instance( ) ) 
		{
			// in order not to spoil the output of gtest :-)
			logBack.setConsoleLevel( LogLevel::LOGLEVEL_UNDEFINED );

			logBack.setLogfileLevel( LogLevel::LOGLEVEL_DATA );
			logBack.setLogfileName( "logTest.log" );
			logBack.setSyslogLevel( LogLevel::LOGLEVEL_DATA );
			logBack.setSyslogFacility( SyslogFacility::WOLFRAME_SYSLOG_FACILITY_USER );
			logBack.setSyslogIdent( "logTest" );			
#if defined( _WIN32_ )
			logBack.setSyslogHost( "localhost" );
			logBack.setSyslogPort( 514 );
			logBack.setEventlogLevel( LogLevel::LOGLEVEL_DATA );
			logBack.setEventlogSource( "logTest" );
			logBack.setEventlogLog( "Application" );
#endif // defined( _WIN32 _ )
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
	LOG_FATAL	<< LogComponent::LogLogging << "fatal error";
	LOG_ALERT	<< LogComponent::LogLogging << "alert";
	LOG_CRITICAL	<< LogComponent::LogLogging << "critical error";
	LOG_SEVERE	<< LogComponent::LogLogging << "severe error";
	LOG_ERROR	<< LogComponent::LogLogging << "an error";
	LOG_WARNING	<< LogComponent::LogLogging << "a warning";
	LOG_NOTICE	<< LogComponent::LogLogging << "a notice";
	LOG_INFO 	<< LogComponent::LogLogging << "an info";
	LOG_DEBUG	<< LogComponent::LogLogging << "debug message";
	LOG_TRACE	<< LogComponent::LogLogging << "debug message with tracing";
	LOG_DATA	<< LogComponent::LogLogging << "debug message with tracing and data";
}

TEST_F( LoggingFixture, LogStandardValues )
{
	std::string s = "a string";
	LOG_INFO << "string: '" << s << "'";
	const char *c = "a C string";
	LOG_INFO << "char constant: '" << c << "'";
	LOG_INFO << "integer: " << 47;
	LOG_INFO << "double: " << 4.71;
}

#ifndef _WIN32
TEST_F( LoggingFixture, LogSystemErrorMarkersUnix )
{
	int fd = open( "bla", O_RDONLY, 0 );
	if( fd == -1 ) {
		LOG_ERROR << "open failed, reason: " << LogError::LogStrerror;
	}
}
#endif

#ifdef _WIN32
TEST_F( LoggingFixture, LogSystemErrorMarkersWin )
{
	OFSTRUCT s;
	HFILE h = OpenFile( "bla", &s, OF_READ );
	if( h == HFILE_ERROR ) {
		LOG_ERROR << "OpenFile failed, reason: " << LogError::LogWinerror;
	}
}
#endif

// fast function
static int f1( int a, int b )
{
	return a+b;
}

// slow function
static int f2( int a, int b )
{
	char *s = (char *)malloc( 100000000 );
	free( s );
	return a+b;
}

TEST_F( LoggingFixture, PerformanceLoggerNoLoggerFastFunc )
{
	// switch off logging code, measure overhead
	logBack.setConsoleLevel( LogLevel::LOGLEVEL_UNDEFINED );

	logBack.setLogfileLevel( LogLevel::LOGLEVEL_UNDEFINED );
	logBack.setSyslogLevel( LogLevel::LOGLEVEL_UNDEFINED );
#if defined( _WIN32_ )
	logBack.setEventlogLevel( LogLevel::LOGLEVEL_UNDEFINED );
#endif // defined( _WIN32 _ )

	for( int i = 0; i < 100000; i++ ) {
		f1( 2, 2 );
	}
}

TEST_F( LoggingFixture, PerformanceLoggerSwitchedOffLoggerFastFunc )
{
	// switch off logging code, measure overhead
	logBack.setConsoleLevel( LogLevel::LOGLEVEL_UNDEFINED );

	logBack.setLogfileLevel( LogLevel::LOGLEVEL_UNDEFINED );
	logBack.setSyslogLevel( LogLevel::LOGLEVEL_UNDEFINED );
#if defined( _WIN32_ )
	logBack.setEventlogLevel( LogLevel::LOGLEVEL_UNDEFINED );
#endif // defined( _WIN32 _ )

	for( int i = 0; i < 100000; i++ ) {
		f1( 2, 2 );
		LOG_TRACE << "test";
	}
}

TEST_F( LoggingFixture, PerformanceLoggerNoLoggerSlowFunc )
{
	// switch off logging code, measure overhead
	logBack.setConsoleLevel( LogLevel::LOGLEVEL_UNDEFINED );

	logBack.setLogfileLevel( LogLevel::LOGLEVEL_UNDEFINED );
	logBack.setSyslogLevel( LogLevel::LOGLEVEL_UNDEFINED );
#if defined( _WIN32_ )
	logBack.setEventlogLevel( LogLevel::LOGLEVEL_UNDEFINED );
#endif // defined( _WIN32 _ )

	for( int i = 0; i < 100000; i++ ) {
		f2( 2, 2 );
	}
}

TEST_F( LoggingFixture, PerformanceLoggerSwitchedOffLoggerSlowFunc )
{
	// switch off logging code, measure overhead
	logBack.setConsoleLevel( LogLevel::LOGLEVEL_UNDEFINED );

	logBack.setLogfileLevel( LogLevel::LOGLEVEL_UNDEFINED );
	logBack.setSyslogLevel( LogLevel::LOGLEVEL_UNDEFINED );
#if defined( _WIN32_ )
	logBack.setEventlogLevel( LogLevel::LOGLEVEL_UNDEFINED );
#endif // defined( _WIN32 _ )

	for( int i = 0; i < 100000; i++ ) {
		f2( 2, 2 );
		LOG_TRACE << "test";
	}
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
