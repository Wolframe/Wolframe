//
// logger.hpp
//

#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

#include "logLevel.hpp"
#include "appConfig.hpp"

#ifndef FAKE_LOGGER

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
#define LOG_DATA	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::LOG_DATA )
#define LOG_TRACE	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::LOG_TRACE )
#define LOG_DEBUG	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::LOG_DEBUG )
#define LOG_INFO	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::LOG_INFO )
#define LOG_NOTICE	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::LOG_NOTICE )
#define LOG_WARNING	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::LOG_WARNING )
#define LOG_ERROR	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::LOG_ERROR )
#define LOG_SEVERE	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::LOG_SEVERE )
#define LOG_CRITICAL	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::LOG_CRITICAL )
#define LOG_ALERT	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::LOG_ALERT )
#define LOG_FATAL	BOOST_LOG_SEV( _SMERP::logger, _SMERP::LogLevel::LOG_FATAL )


#else // FAKE_LOGGER

#include <ostream>

namespace _SMERP {

	class LogBackend
	{
	public:
		LogBackend()	{ logLevel_ = _SMERP::LogLevel::LOG_NOTICE; }
		~LogBackend()	{ /* free logger resources here */ };

		void setLevel( LogLevel::Level level )	{
			logLevel_ = level;
		}
		void log( LogLevel::Level level, const std::string& msg )	{
			if ( level >= logLevel_ )
				std::cerr << level << ": " << msg << std::endl;
		}
	private:
		LogLevel::Level	logLevel_;
	};


	class Logger {
	public:
		Logger( LogBackend& backend ) :	logBk_( backend )	{}
		~Logger()			{ logBk_.log( msgLevel_, os_.str()); }

		std::ostringstream& Get( LogLevel::Level level )	{
			msgLevel_ = level;
			return os_;
		}

	protected:
		std::ostringstream os_;
	private:
		LogBackend&	logBk_;
		LogLevel::Level	msgLevel_;

		Logger();
		Logger( const Logger& );
		Logger& operator= ( const Logger& );
	};
} // namespace _SMERP

extern _SMERP::LogBackend	logBack;

// shortcut macros
#define LOG_DATA	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOG_DATA )
#define LOG_TRACE	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOG_TRACE )
#define LOG_DEBUG	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOG_DEBUG )
#define LOG_INFO	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOG_INFO )
#define LOG_NOTICE	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOG_NOTICE )
#define LOG_WARNING	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOG_WARNING )
#define LOG_ERROR	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOG_ERROR )
#define LOG_SEVERE	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOG_SEVERE )
#define LOG_CRITICAL	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOG_CRITICAL )
#define LOG_ALERT	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOG_ALERT )
#define LOG_FATAL	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOG_FATAL )

#endif // FAKE_LOGGER

#endif // _LOGGER_HPP_INCLUDED
