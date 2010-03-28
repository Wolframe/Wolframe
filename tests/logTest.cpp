#include <iostream>

#include <boost/log/common.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/filters.hpp>

#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/to_console.hpp>
#ifndef _WIN32
#include <boost/log/utility/init/to_syslog.hpp>
#else
#include <boost/log/utility/init/to_eventlog.hpp>
#endif
#include <boost/log/utility/init/common_attributes.hpp>

namespace logging = boost::log;
namespace flt = boost::log::filters;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;
namespace fmt = boost::log::formatters;
namespace sinks = boost::log::sinks;

using namespace std;

/* our own log level enum */
enum log_level {
	FATAL,
	ALERT,
	CRITICAL,
	ERROR,
	WARNING,
	NOTICE,
	INFO,
	DEBUG0,
	DEBUG1,
	DEBUG2,
	DEBUG3,
	DEBUG4,
	DEBUG5
};

/* define shortcuts to get the current logger with a given log level */
#define LOG_FATAL	BOOST_LOG_SEV( logger, FATAL )
#define LOG_ALERT	BOOST_LOG_SEV( logger, ALERT )
#define LOG_CRITICAL	BOOST_LOG_SEV( logger, CRITICAL )
#define LOG_ERROR	BOOST_LOG_SEV( logger, ERROR )
#define LOG_WARNING	BOOST_LOG_SEV( logger, WARNING )
#define LOG_NOTICE 	BOOST_LOG_SEV( logger, NOTICE )
#define LOG_INFO 	BOOST_LOG_SEV( logger, INFO )
#define LOG_DEBUG0	BOOST_LOG_SEV( logger, DEBUG0 )
#define LOG_DEBUG1	BOOST_LOG_SEV( logger, DEBUG1 )
#define LOG_DEBUG2	BOOST_LOG_SEV( logger, DEBUG2 )
#define LOG_DEBUG3	BOOST_LOG_SEV( logger, DEBUG3 )
#define LOG_DEBUG4	BOOST_LOG_SEV( logger, DEBUG4 )
#define LOG_DEBUG5	BOOST_LOG_SEV( logger, DEBUG5 )

template< typename CharT, typename TraitsT >
inline basic_ostream< CharT, TraitsT > &operator<< ( basic_ostream< CharT, TraitsT >& s, log_level l ) {
	static const char *const str[] = {
		"FATAL", "ALERT", "CRITICAL", "ERROR", "WARNING", "NOTICE", "INFO",
		"DEBUG0", "DEBUG1", "DEBUG2", "DEBUG3", "DEBUG4", "DEBUG5" };
	if( static_cast< size_t >( l ) < ( sizeof( str ) / sizeof( *str ) ) ) {
		s << str[l];
	} else {
		s << "Unknown enum used '" << static_cast< int >( l ) << "'";
	}
	return s;
}

int main( void ) {
	logging::init_log_to_console(
		clog,
		keywords::format = fmt::format( "%1% %2%: %3%" )
			% fmt::date_time( "TimeStamp", nothrow )
			% fmt::attr< log_level >( "Severity", nothrow )
			% fmt::message( )
	);

	logging::init_log_to_file(
		keywords::file_name = "logTest.log",
		keywords::open_mode = ( ios_base::out | ios_base::app ),
		keywords::filter = flt::attr< log_level >( "Severity", nothrow ) <= NOTICE,
		keywords::format = fmt::format( "%1% %2%: %3%" )
			% fmt::date_time( "TimeStamp", nothrow )
			% fmt::attr< log_level >( "Severity", nothrow )
			% fmt::message( )
	);

#ifndef _WIN32
	sinks::syslog::custom_severity_mapping< log_level > mapping( "Severity" );
	mapping[FATAL] = sinks::syslog::emergency;
	mapping[ALERT] = sinks::syslog::alert;
	mapping[CRITICAL] = sinks::syslog::critical;
	mapping[ERROR] = sinks::syslog::error;
	mapping[WARNING] = sinks::syslog::warning;
	mapping[NOTICE] = sinks::syslog::notice;
	mapping[INFO] = sinks::syslog::info;
	mapping[DEBUG0] = sinks::syslog::debug;
	mapping[DEBUG1] = sinks::syslog::debug;
	mapping[DEBUG2] = sinks::syslog::debug;
	mapping[DEBUG3] = sinks::syslog::debug;
	mapping[DEBUG4] = sinks::syslog::debug;
	mapping[DEBUG5] = sinks::syslog::debug;

	logging::init_log_to_syslog(
		keywords::facility = sinks::syslog::user,
		keywords::use_impl = sinks::syslog::native,
		keywords::custom_severity_mapping = mapping,
		keywords::filter = flt::attr< log_level >( "Severity", nothrow ) <= DEBUG5
	);
#else
	sinks::event_log::custom_event_type_mapping< log_level > mapping( "Severity" );
	mapping[FATAL] = sinks::event_log::error;
	mapping[ALERT] = sinks::event_log::error;
	mapping[CRITICAL] = sinks::event_log::error;
	mapping[ERROR] = sinks::event_log::error;
	mapping[WARNING] = sinks::event_log::warning;
	mapping[NOTICE] = sinks::event_log::info;
	mapping[INFO] = sinks::event_log::info;
	mapping[DEBUG0] = sinks::event_log::info;
	mapping[DEBUG1] = sinks::event_log::info;
	mapping[DEBUG2] = sinks::event_log::info;
	mapping[DEBUG3] = sinks::event_log::info;
	mapping[DEBUG4] = sinks::event_log::info;
	mapping[DEBUG5] = sinks::event_log::info;

	logging::init_log_to_eventlog(
		keywords::log_source = "Boosttests Logtest 1",
		keywords::custom_event_type_mapping = mapping,
		keywords::filter = flt::attr< log_level >( "Severity", nothrow ) <= DEBUG5
	);
#endif

	logging::add_common_attributes( );

	src::severity_logger< log_level > logger;

	LOG_FATAL	<< "fatal error";
	LOG_ALERT	<< "alert";
	LOG_CRITICAL	<< "critical error";
	LOG_ERROR	<< "an error";
	LOG_WARNING	<< "a warning";
	LOG_NOTICE	<< "a notice";
	LOG_INFO 	<< "an info";
	LOG_DEBUG0	<< "debug message of level 0";
	LOG_DEBUG1	<< "debug message of level 1";
	LOG_DEBUG2	<< "debug message of level 2";
	LOG_DEBUG3	<< "debug message of level 3";
	LOG_DEBUG4	<< "debug message of level 4";
	LOG_DEBUG5	<< "debug message of level 5";

	return 0;
}
