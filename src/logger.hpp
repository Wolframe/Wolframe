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

// small compromise in leaking internal implementation of the logger to the
// outside world. sadly boost log is a little bit intrusive because of the
// macros you have to use..
extern src::severity_logger< _SMERP::Logger::LogLevel > logger;

} // namespace _SMERP

// shortcut macros
#define LOG_DATA	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::DATA )
#define LOG_DEBUG	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::DEBUG )
#define LOG_TRACE	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::DATA )
#define LOG_DEBUG	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::DEBUG )
#define LOG_INFO	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::INFO )
#define LOG_NOTICE	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::NOTICE )
#define LOG_WARNING	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::WARNING )
#define LOG_ERROR	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::ERROR )
#define LOG_SEVERE	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::SEVERE )
#define LOG_CRITICAL	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::CRITICAL )
#define LOG_ALERT	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::ALERT )
#define LOG_FATAL	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::FATAL )


#endif // _LOGGER_HPP_INCLUDED
