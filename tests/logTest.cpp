#include <iostream>

#include <boost/log/common.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/filters.hpp>

#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/common_attributes.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/log/sinks.hpp>

#include <boost/preprocessor/comparison/greater.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_shifted_params.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>

#include <boost/log/unused.hpp>

namespace logging = boost::log;
namespace flt = boost::log::filters;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;
namespace fmt = boost::log::formatters;
namespace sinks = boost::log::sinks;

using namespace std;

using boost::shared_ptr;
using boost::make_shared;

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

#ifndef _WIN32

//! \cond
#ifndef BOOST_LOG_NO_THREADS
#define BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL sinks::synchronous_sink
#else
#define BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL sinks::unlocked_sink
#endif
//! \endcond

namespace aux {

template< typename CharT, typename ArgsT >
	shared_ptr< BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL<
		sinks::basic_syslog_backend< CharT >
	>
> init_log_to_syslog( ArgsT const& args UNUSED ) {
	typedef sinks::basic_syslog_backend< CharT > backend_t;
	shared_ptr< backend_t > backend = make_shared< backend_t >( args );

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
	backend->set_severity_mapper( mapping );

	typedef BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL< backend_t > sink_t;
	shared_ptr< sink_t > sink = make_shared< sink_t >( backend );

	shared_ptr< logging::core > core = logging::core::get( );
	core->add_sink( sink );

	return sink;
}

} // namespace aux

#define BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL(z, n, data)\
    template< typename CharT, BOOST_PP_ENUM_PARAMS(n, typename T) >\
    inline shared_ptr<\
    	BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL<\
    		sinks::basic_syslog_backend< CharT >\
    	>\
    > init_log_to_syslog(BOOST_PP_ENUM_BINARY_PARAMS(n, T, const& arg))\
    {\
        return aux::init_log_to_syslog< CharT >((\
            arg0\
            BOOST_PP_COMMA_IF(BOOST_PP_GREATER(n, 1))\
            BOOST_PP_ENUM_SHIFTED_PARAMS(n, arg)\
        ));\
    }

BOOST_PP_REPEAT_FROM_TO(1, BOOST_LOG_MAX_PARAMETER_ARGS, BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL, ~)

#undef BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL

#if defined(BOOST_LOG_USE_CHAR)

#define BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL(z, n, data)\
    template< BOOST_PP_ENUM_PARAMS(n, typename T) >\
    inline shared_ptr<\
        BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL<\
            sinks::syslog_backend\
        >\
    > init_log_to_syslog(BOOST_PP_ENUM_BINARY_PARAMS(n, T, const& arg))\
    {\
        return init_log_to_syslog< char >(BOOST_PP_ENUM_PARAMS(n, arg));\
    }

BOOST_PP_REPEAT_FROM_TO(1, BOOST_LOG_MAX_PARAMETER_ARGS, BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL, ~)

#undef BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL

#endif // defined(BOOST_LOG_USE_CHAR)

#if defined(BOOST_LOG_USE_WCHAR_T)

#define BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL(z, n, data)\
    template< BOOST_PP_ENUM_PARAMS(n, typename T) >\
    inline shared_ptr<\
        BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL<\
            sinks::wsyslog_backend\
        >\
    > winit_log_to_file(BOOST_PP_ENUM_BINARY_PARAMS(n, T, const& arg))\
    {\
        return init_log_to_syslog< wchar_t >(BOOST_PP_ENUM_PARAMS(n, arg));\
    }

BOOST_PP_REPEAT_FROM_TO(1, BOOST_LOG_MAX_PARAMETER_ARGS, BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL, ~)

#undef BOOST_LOG_INIT_LOG_TO_FILE_INTERNAL

#endif // defined(BOOST_LOG_USE_WCHAR_T)

#endif // not defined( _WIN32 )

#ifdef _WIN32

//! \cond
#ifndef BOOST_LOG_NO_THREADS
#define BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL sinks::synchronous_sink
#else
#define BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL sinks::unlocked_sink
#endif
//! \endcond

shared_ptr< BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL< sinks::simple_event_log_backend > > init_log_to_eventlog( ) {
	typedef sinks::simple_event_log_backend backend_t;
	shared_ptr< backend_t > backend = make_shared< backend_t >(
		keywords::log_source = "Boosttests Logtest 1",
		keywords::filter = flt::attr< log_level >( "Severity", nothrow ) <= NOTICE
	);
	
	typedef BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL< backend_t > sink_t;
	shared_ptr< sink_t > sink = make_shared< sink_t >( backend );
	
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
	sink->locked_backend( )->set_event_type_mapper( mapping );
	
	shared_ptr< logging::core > core = logging::core::get( );
	core->add_sink( sink );

	return sink;
}
#endif

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
	init_log_to_syslog(
		keywords::facility = sinks::syslog::user,
		keywords::use_impl = sinks::syslog::native,
		keywords::filter = flt::attr< log_level >( "Severity", nothrow ) <= FATAL
	);
#else
	init_log_to_eventlog(
	);
#endif

	logging::add_common_attributes( );

	src::severity_logger< log_level > logger;

	LOG_FATAL	<< "fatal error";
	LOG_ERROR	<< "an error";
	LOG_WARNING	<< "a warning";
	LOG_NOTICE	<< "a notice";
	LOG_INFO 	<< "an info";
	LOG_DEBUG4	<< "debug message of level 4";

	return 0;
}
