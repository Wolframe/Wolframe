//
// testing the Logging library
//

#include "logger-v1.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include <stdexcept>

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
public:
	void setLevel( const LogLevel::Level level )	{
		// in order not to spoil the output of gtest :-)
		logBack.setConsoleLevel( LogLevel::LOGLEVEL_UNDEFINED );

		logBack.setLogfileLevel( level );
		logBack.setSyslogLevel( level );
#if defined( _WIN32 )
		logBack.setWinDebugLevel( level );
		logBack.setEventlogLevel( level );
#endif // defined( _WIN32 )

	}

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
#if defined( _WIN32 )
		// mmh? they disappeared
		//			logBack.setSyslogHost( "localhost" );
		//			logBack.setSyslogPort( 514 );
		logBack.setWinDebugLevel( LogLevel::LOGLEVEL_DATA );
		logBack.setEventlogLevel( LogLevel::LOGLEVEL_DATA );
		logBack.setEventlogSource( "wolframe" );
		logBack.setEventlogLog( "Application" );
#endif // defined( _WIN32 )
	}

protected:
	LogBackend& logBack;
};


// test classes for log behavior
// testA throws logic_error when the constructor is called
class testA	{
public:
	testA()	{ throw std::logic_error( "testA constructor called" ); }
	friend std::ostream& operator << ( std::ostream& o, const testA& )
		{ o << "ouput testA class"; return o; }
};

// testB throws runtime_error when the << operator is called
class testB	{
public:
//	testB()	{ std::cerr << "testB constructor called"; }

	friend std::ostream& operator << ( std::ostream&, const testB& )
		{  throw std::runtime_error( "testB << operator called" ); }
};


TEST_F( LoggingFixture, LogMacros )
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


TEST_F( LoggingFixture, LogLevelData )
{
	setLevel( LogLevel::LOGLEVEL_DATA );
	EXPECT_THROW( LOG_FATAL << testA(), std::logic_error );
	EXPECT_THROW( LOG_ALERT <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_CRITICAL <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_SEVERE <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_ERROR <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_WARNING <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_NOTICE <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_INFO <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_DEBUG <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_TRACE <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_DATA <<  testA(), std::logic_error );

	EXPECT_THROW( LOG_FATAL << testB(), std::runtime_error );
	EXPECT_THROW( LOG_ALERT <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_CRITICAL <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_SEVERE <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_ERROR <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_WARNING <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_NOTICE <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_INFO <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_DEBUG <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_TRACE <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_DATA <<  testB(), std::runtime_error );
}

TEST_F( LoggingFixture, LogLevelTrace )
{
	setLevel( LogLevel::LOGLEVEL_TRACE );
	EXPECT_THROW( LOG_FATAL << testA(), std::logic_error );
	EXPECT_THROW( LOG_ALERT <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_CRITICAL <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_SEVERE <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_ERROR <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_WARNING <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_NOTICE <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_INFO <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_DEBUG <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_TRACE <<  testA(), std::logic_error );
	EXPECT_NO_THROW( LOG_DATA <<  testA() );

	EXPECT_THROW( LOG_FATAL << testB(), std::runtime_error );
	EXPECT_THROW( LOG_ALERT <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_CRITICAL <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_SEVERE <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_ERROR <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_WARNING <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_NOTICE <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_INFO <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_DEBUG <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_TRACE <<  testB(), std::runtime_error );
	EXPECT_NO_THROW( LOG_DATA <<  testB() );
}

TEST_F( LoggingFixture, LogLevelDebug )
{
	setLevel( LogLevel::LOGLEVEL_DEBUG );
	EXPECT_THROW( LOG_FATAL << testA(), std::logic_error );
	EXPECT_THROW( LOG_ALERT <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_CRITICAL <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_SEVERE <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_ERROR <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_WARNING <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_NOTICE <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_INFO <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_DEBUG <<  testA(), std::logic_error );
	EXPECT_NO_THROW( LOG_TRACE <<  testA() );
	EXPECT_NO_THROW( LOG_DATA <<  testA() );

	EXPECT_THROW( LOG_FATAL << testB(), std::runtime_error );
	EXPECT_THROW( LOG_ALERT <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_CRITICAL <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_SEVERE <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_ERROR <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_WARNING <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_NOTICE <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_INFO <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_DEBUG <<  testB(), std::runtime_error );
	EXPECT_NO_THROW( LOG_TRACE <<  testB() );
	EXPECT_NO_THROW( LOG_DATA <<  testB() );
}

TEST_F( LoggingFixture, LogLevelInfo )
{
	setLevel( LogLevel::LOGLEVEL_INFO );
	EXPECT_THROW( LOG_FATAL << testA(), std::logic_error );
	EXPECT_THROW( LOG_ALERT <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_CRITICAL <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_SEVERE <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_ERROR <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_WARNING <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_NOTICE <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_INFO <<  testA(), std::logic_error );
	EXPECT_NO_THROW( LOG_DEBUG <<  testA() );
	EXPECT_NO_THROW( LOG_TRACE <<  testA() );
	EXPECT_NO_THROW( LOG_DATA <<  testA() );

	EXPECT_THROW( LOG_FATAL << testB(), std::runtime_error );
	EXPECT_THROW( LOG_ALERT <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_CRITICAL <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_SEVERE <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_ERROR <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_WARNING <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_NOTICE <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_INFO <<  testB(), std::runtime_error );
	EXPECT_NO_THROW( LOG_DEBUG <<  testB() );
	EXPECT_NO_THROW( LOG_TRACE <<  testB() );
	EXPECT_NO_THROW( LOG_DATA <<  testB() );
}

TEST_F( LoggingFixture, LogLevelNotice )
{
	setLevel( LogLevel::LOGLEVEL_NOTICE );
	EXPECT_THROW( LOG_FATAL << testA(), std::logic_error );
	EXPECT_THROW( LOG_ALERT <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_CRITICAL <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_SEVERE <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_ERROR <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_WARNING <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_NOTICE <<  testA(), std::logic_error );
	EXPECT_NO_THROW( LOG_INFO <<  testA() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testA() );
	EXPECT_NO_THROW( LOG_TRACE <<  testA() );
	EXPECT_NO_THROW( LOG_DATA <<  testA() );

	EXPECT_THROW( LOG_FATAL << testB(), std::runtime_error );
	EXPECT_THROW( LOG_ALERT <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_CRITICAL <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_SEVERE <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_ERROR <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_WARNING <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_NOTICE <<  testB(), std::runtime_error );
	EXPECT_NO_THROW( LOG_INFO <<  testB() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testB() );
	EXPECT_NO_THROW( LOG_TRACE <<  testB() );
	EXPECT_NO_THROW( LOG_DATA <<  testB() );
}

TEST_F( LoggingFixture, LogLevelWarning )
{
	setLevel( LogLevel::LOGLEVEL_WARNING );
	EXPECT_THROW( LOG_FATAL << testA(), std::logic_error );
	EXPECT_THROW( LOG_ALERT <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_CRITICAL <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_SEVERE <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_ERROR <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_WARNING <<  testA(), std::logic_error );
	EXPECT_NO_THROW( LOG_NOTICE <<  testA() );
	EXPECT_NO_THROW( LOG_INFO <<  testA() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testA() );
	EXPECT_NO_THROW( LOG_TRACE <<  testA() );
	EXPECT_NO_THROW( LOG_DATA <<  testA() );

	EXPECT_THROW( LOG_FATAL << testB(), std::runtime_error );
	EXPECT_THROW( LOG_ALERT <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_CRITICAL <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_SEVERE <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_ERROR <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_WARNING <<  testB(), std::runtime_error );
	EXPECT_NO_THROW( LOG_NOTICE <<  testB() );
	EXPECT_NO_THROW( LOG_INFO <<  testB() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testB() );
	EXPECT_NO_THROW( LOG_TRACE <<  testB() );
	EXPECT_NO_THROW( LOG_DATA <<  testB() );
}

TEST_F( LoggingFixture, LogLevelError )
{
	setLevel( LogLevel::LOGLEVEL_ERROR );
	EXPECT_THROW( LOG_FATAL << testA(), std::logic_error );
	EXPECT_THROW( LOG_ALERT <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_CRITICAL <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_SEVERE <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_ERROR <<  testA(), std::logic_error );
	EXPECT_NO_THROW( LOG_WARNING <<  testA() );
	EXPECT_NO_THROW( LOG_NOTICE <<  testA() );
	EXPECT_NO_THROW( LOG_INFO <<  testA() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testA() );
	EXPECT_NO_THROW( LOG_TRACE <<  testA() );
	EXPECT_NO_THROW( LOG_DATA <<  testA() );

	EXPECT_THROW( LOG_FATAL << testB(), std::runtime_error );
	EXPECT_THROW( LOG_ALERT <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_CRITICAL <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_SEVERE <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_ERROR <<  testB(), std::runtime_error );
	EXPECT_NO_THROW( LOG_WARNING <<  testB() );
	EXPECT_NO_THROW( LOG_NOTICE <<  testB() );
	EXPECT_NO_THROW( LOG_INFO <<  testB() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testB() );
	EXPECT_NO_THROW( LOG_TRACE <<  testB() );
	EXPECT_NO_THROW( LOG_DATA <<  testB() );
}

TEST_F( LoggingFixture, LogLevelSevere )
{
	setLevel( LogLevel::LOGLEVEL_SEVERE );
	EXPECT_THROW( LOG_FATAL << testA(), std::logic_error );
	EXPECT_THROW( LOG_ALERT <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_CRITICAL <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_SEVERE <<  testA(), std::logic_error );
	EXPECT_NO_THROW( LOG_ERROR <<  testA() );
	EXPECT_NO_THROW( LOG_WARNING <<  testA() );
	EXPECT_NO_THROW( LOG_NOTICE <<  testA() );
	EXPECT_NO_THROW( LOG_INFO <<  testA() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testA() );
	EXPECT_NO_THROW( LOG_TRACE <<  testA() );
	EXPECT_NO_THROW( LOG_DATA <<  testA() );

	EXPECT_THROW( LOG_FATAL << testB(), std::runtime_error );
	EXPECT_THROW( LOG_ALERT <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_CRITICAL <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_SEVERE <<  testB(), std::runtime_error );
	EXPECT_NO_THROW( LOG_ERROR <<  testB() );
	EXPECT_NO_THROW( LOG_WARNING <<  testB() );
	EXPECT_NO_THROW( LOG_NOTICE <<  testB() );
	EXPECT_NO_THROW( LOG_INFO <<  testB() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testB() );
	EXPECT_NO_THROW( LOG_TRACE <<  testB() );
	EXPECT_NO_THROW( LOG_DATA <<  testB() );
}

TEST_F( LoggingFixture, LogLevelCritical )
{
	setLevel( LogLevel::LOGLEVEL_CRITICAL );
	EXPECT_THROW( LOG_FATAL << testA(), std::logic_error );
	EXPECT_THROW( LOG_ALERT <<  testA(), std::logic_error );
	EXPECT_THROW( LOG_CRITICAL <<  testA(), std::logic_error );
	EXPECT_NO_THROW( LOG_SEVERE <<  testA() );
	EXPECT_NO_THROW( LOG_ERROR <<  testA() );
	EXPECT_NO_THROW( LOG_WARNING <<  testA() );
	EXPECT_NO_THROW( LOG_NOTICE <<  testA() );
	EXPECT_NO_THROW( LOG_INFO <<  testA() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testA() );
	EXPECT_NO_THROW( LOG_TRACE <<  testA() );
	EXPECT_NO_THROW( LOG_DATA <<  testA() );

	EXPECT_THROW( LOG_FATAL << testB(), std::runtime_error );
	EXPECT_THROW( LOG_ALERT <<  testB(), std::runtime_error );
	EXPECT_THROW( LOG_CRITICAL <<  testB(), std::runtime_error );
	EXPECT_NO_THROW( LOG_SEVERE <<  testB() );
	EXPECT_NO_THROW( LOG_ERROR <<  testB() );
	EXPECT_NO_THROW( LOG_WARNING <<  testB() );
	EXPECT_NO_THROW( LOG_NOTICE <<  testB() );
	EXPECT_NO_THROW( LOG_INFO <<  testB() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testB() );
	EXPECT_NO_THROW( LOG_TRACE <<  testB() );
	EXPECT_NO_THROW( LOG_DATA <<  testB() );
}

TEST_F( LoggingFixture, LogLevelAlert )
{
	setLevel( LogLevel::LOGLEVEL_ALERT );
	EXPECT_THROW( LOG_FATAL << testA(), std::logic_error );
	EXPECT_THROW( LOG_ALERT <<  testA(), std::logic_error );
	EXPECT_NO_THROW( LOG_CRITICAL <<  testA() );
	EXPECT_NO_THROW( LOG_SEVERE <<  testA() );
	EXPECT_NO_THROW( LOG_ERROR <<  testA() );
	EXPECT_NO_THROW( LOG_WARNING <<  testA() );
	EXPECT_NO_THROW( LOG_NOTICE <<  testA() );
	EXPECT_NO_THROW( LOG_INFO <<  testA() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testA() );
	EXPECT_NO_THROW( LOG_TRACE <<  testA() );
	EXPECT_NO_THROW( LOG_DATA <<  testA() );

	EXPECT_THROW( LOG_FATAL << testB(), std::runtime_error );
	EXPECT_THROW( LOG_ALERT <<  testB(), std::runtime_error );
	EXPECT_NO_THROW( LOG_CRITICAL <<  testB() );
	EXPECT_NO_THROW( LOG_SEVERE <<  testB() );
	EXPECT_NO_THROW( LOG_ERROR <<  testB() );
	EXPECT_NO_THROW( LOG_WARNING <<  testB() );
	EXPECT_NO_THROW( LOG_NOTICE <<  testB() );
	EXPECT_NO_THROW( LOG_INFO <<  testB() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testB() );
	EXPECT_NO_THROW( LOG_TRACE <<  testB() );
	EXPECT_NO_THROW( LOG_DATA <<  testB() );
}

TEST_F( LoggingFixture, LogLevelFatal )
{
	setLevel( LogLevel::LOGLEVEL_FATAL );
	EXPECT_THROW( LOG_FATAL << testA(), std::logic_error );
	EXPECT_NO_THROW( LOG_ALERT <<  testA() );
	EXPECT_NO_THROW( LOG_CRITICAL <<  testA() );
	EXPECT_NO_THROW( LOG_SEVERE <<  testA() );
	EXPECT_NO_THROW( LOG_ERROR <<  testA() );
	EXPECT_NO_THROW( LOG_WARNING <<  testA() );
	EXPECT_NO_THROW( LOG_NOTICE <<  testA() );
	EXPECT_NO_THROW( LOG_INFO <<  testA() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testA() );
	EXPECT_NO_THROW( LOG_TRACE <<  testA() );
	EXPECT_NO_THROW( LOG_DATA <<  testA() );

	EXPECT_THROW( LOG_FATAL << testB(), std::runtime_error );
	EXPECT_NO_THROW( LOG_ALERT <<  testB() );
	EXPECT_NO_THROW( LOG_CRITICAL <<  testB() );
	EXPECT_NO_THROW( LOG_SEVERE <<  testB() );
	EXPECT_NO_THROW( LOG_ERROR <<  testB() );
	EXPECT_NO_THROW( LOG_WARNING <<  testB() );
	EXPECT_NO_THROW( LOG_NOTICE <<  testB() );
	EXPECT_NO_THROW( LOG_INFO <<  testB() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testB() );
	EXPECT_NO_THROW( LOG_TRACE <<  testB() );
	EXPECT_NO_THROW( LOG_DATA <<  testB() );
}

TEST_F( LoggingFixture, LogLevelUndefined )
{
	setLevel( LogLevel::LOGLEVEL_UNDEFINED );
	EXPECT_NO_THROW( LOG_FATAL << testA() );
	EXPECT_NO_THROW( LOG_ALERT <<  testA() );
	EXPECT_NO_THROW( LOG_CRITICAL <<  testA() );
	EXPECT_NO_THROW( LOG_SEVERE <<  testA() );
	EXPECT_NO_THROW( LOG_ERROR <<  testA() );
	EXPECT_NO_THROW( LOG_WARNING <<  testA() );
	EXPECT_NO_THROW( LOG_NOTICE <<  testA() );
	EXPECT_NO_THROW( LOG_INFO <<  testA() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testA() );
	EXPECT_NO_THROW( LOG_TRACE <<  testA() );
	EXPECT_NO_THROW( LOG_DATA <<  testA() );

	EXPECT_NO_THROW( LOG_FATAL << testB() );
	EXPECT_NO_THROW( LOG_ALERT <<  testB() );
	EXPECT_NO_THROW( LOG_CRITICAL <<  testB() );
	EXPECT_NO_THROW( LOG_SEVERE <<  testB() );
	EXPECT_NO_THROW( LOG_ERROR <<  testB() );
	EXPECT_NO_THROW( LOG_WARNING <<  testB() );
	EXPECT_NO_THROW( LOG_NOTICE <<  testB() );
	EXPECT_NO_THROW( LOG_INFO <<  testB() );
	EXPECT_NO_THROW( LOG_DEBUG <<  testB() );
	EXPECT_NO_THROW( LOG_TRACE <<  testB() );
	EXPECT_NO_THROW( LOG_DATA <<  testB() );
}


int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
