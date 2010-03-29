#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

#include <boost/log/common.hpp>

namespace src = boost::log::sources;

namespace _SMERP {

class Logger {
public:
	enum LogLevel {
		ALWAYS,		/// always log
		DATA,		/// log everything, including data
		TRACE,		/// trace functions calls
		DEBUG,		/// log operations
		INFO,
		NOTICE,
		WARNING,
		ERROR,
		SEVERE,
		CRITICAL,
		ALERT,
		FATAL,
		NEVER		/// log this only under special conditions
	};

	static void initialize( );

};

extern src::severity_logger< _SMERP::Logger::LogLevel > logger;

} // namespace _SMERP



#define LOG_DEBUG      BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::DEBUG )

#endif // _LOGGER_HPP_INCLUDED
