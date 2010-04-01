#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

#include "appConfig.hpp"

#include <string>

#include <boost/log/common.hpp>

namespace src = boost::log::sources;

namespace _SMERP {

class Logger {
public:
	enum LogLevel {
		_SMERP_ALWAYS,		/// always log
		_SMERP_DATA,		/// log everything, including data
		_SMERP_TRACE,		/// trace functions calls
		_SMERP_DEBUG,		/// log operations
		_SMERP_INFO,
		_SMERP_NOTICE,
		_SMERP_WARNING,
		_SMERP_ERROR,
		_SMERP_SEVERE,
		_SMERP_CRITICAL,
		_SMERP_ALERT,
		_SMERP_FATAL,
		_SMERP_NEVER		/// log this only under special conditions
	};

	static LogLevel str2LogLevel( const std::string s );

	static void initialize( const ApplicationConfiguration& config );
};

// small compromise in leaking internal implementation of the logger to the
// outside world. sadly boost log is a little bit intrusive because of the
// macros you have to use..
extern src::severity_logger< _SMERP::Logger::LogLevel > logger;

} // namespace _SMERP

// shortcut macros
#define LOG_DATA	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::_SMERP_DATA )
#define LOG_DEBUG	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::_SMERP_DEBUG )
#define LOG_TRACE	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::_SMERP_DATA )
#define LOG_DEBUG	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::_SMERP_DEBUG )
#define LOG_INFO	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::_SMERP_INFO )
#define LOG_NOTICE	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::_SMERP_NOTICE )
#define LOG_WARNING	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::_SMERP_WARNING )
#define LOG_ERROR	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::_SMERP_ERROR )
#define LOG_SEVERE	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::_SMERP_SEVERE )
#define LOG_CRITICAL	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::_SMERP_CRITICAL )
#define LOG_ALERT	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::_SMERP_ALERT )
#define LOG_FATAL	BOOST_LOG_SEV( _SMERP::logger, _SMERP::Logger::_SMERP_FATAL )


#endif // _LOGGER_HPP_INCLUDED
