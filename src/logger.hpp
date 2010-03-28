#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

#include "singleton.hpp"

namespace _SMERP {

class Logger : public Singleton<Logger> {
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

	src::severity_logger< LogLevel > logger;

protected:
	Logger( );
};

#define LOG_DEBUG	BOOST_LOG_SEV( Logger::logger, LogLevel::DEBUG );

} // namespace _SMERP

#endif // _LOGGER_HPP_INCLUDED
