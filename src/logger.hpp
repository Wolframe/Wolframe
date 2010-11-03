//
// logger.hpp
//

#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

#include "logLevel.hpp"
#include "appConfig.hpp"

#include <ostream>

namespace _SMERP {

	class LogBackend
	{
	public:
		LogBackend()	{ logLevel_ = _SMERP::LogLevel::LOGLEVEL_NOTICE; }
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
#define LOG_DATA	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOGLEVEL_DATA )
#define LOG_TRACE	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOGLEVEL_TRACE )
#define LOG_DEBUG	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOGLEVEL_DEBUG )
#define LOG_INFO	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOGLEVEL_INFO )
#define LOG_NOTICE	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOGLEVEL_NOTICE )
#define LOG_WARNING	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOGLEVEL_WARNING )
#define LOG_ERROR	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOGLEVEL_ERROR )
#define LOG_SEVERE	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOGLEVEL_SEVERE )
#define LOG_CRITICAL	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOGLEVEL_CRITICAL )
#define LOG_ALERT	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOGLEVEL_ALERT )
#define LOG_FATAL	_SMERP::Logger( logBack ).Get( _SMERP::LogLevel::LOGLEVEL_FATAL )

#endif // _LOGGER_HPP_INCLUDED
