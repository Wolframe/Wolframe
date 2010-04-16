//
// logger.hpp
//

#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

#include "logLevel.hpp"
#include "appConfig.hpp"

#include <string>

#include <boost/log/common.hpp>

namespace src = boost::log::sources;

namespace _SMERP {

class Logger {
public:
	static void initialize( const ApplicationConfiguration& config );
};

// small compromise in leaking internal implementation of the logger to the
// outside world. sadly boost log is a little bit intrusive because of the
// macros you have to use..
extern src::severity_logger< _SMERP::LogLevel::Level > logger;

} // namespace _SMERP

// shortcut macros
#define LOG_DATA	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::_SMERP_DATA )
#define LOG_TRACE	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::_SMERP_TRACE )
#define LOG_DEBUG	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::_SMERP_DEBUG )
#define LOG_INFO	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::_SMERP_INFO )
#define LOG_NOTICE	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::_SMERP_NOTICE )
#define LOG_WARNING	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::_SMERP_WARNING )
#define LOG_ERROR	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::_SMERP_ERROR )
#define LOG_SEVERE	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::_SMERP_SEVERE )
#define LOG_CRITICAL	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::_SMERP_CRITICAL )
#define LOG_ALERT	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::_SMERP_ALERT )
#define LOG_FATAL	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::_SMERP_FATAL )


#endif // _LOGGER_HPP_INCLUDED
