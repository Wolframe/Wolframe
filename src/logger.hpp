//
// logger.hpp
//

#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

#ifndef FAKE_LOGGER

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

#else // FAKE_LOGGER

#include "appConfig.hpp"

#include <ostream>

namespace _SMERP {

class Logger {
public:
	static void initialize( const ApplicationConfiguration& config )	{ (void)(config); }
	static std::ostream& Get()	{ return std::cerr; }
};

} // namespace _SMERP

// shortcut macros
#define LOG_DATA	_SMERP::Logger::Get()
#define LOG_TRACE	_SMERP::Logger::Get()
#define LOG_DEBUG	_SMERP::Logger::Get()
#define LOG_INFO	_SMERP::Logger::Get()
#define LOG_NOTICE	_SMERP::Logger::Get()
#define LOG_WARNING	_SMERP::Logger::Get()
#define LOG_ERROR	_SMERP::Logger::Get()
#define LOG_SEVERE	_SMERP::Logger::Get()
#define LOG_CRITICAL	_SMERP::Logger::Get()
#define LOG_ALERT	_SMERP::Logger::Get()
#define LOG_FATAL	_SMERP::Logger::Get()

#endif // FAKE_LOGGER

#endif // _LOGGER_HPP_INCLUDED
