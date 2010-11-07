//
// logger.hpp
//

#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

#include "logLevel.hpp"
#include "appConfig.hpp"

#include <ostream>

#ifndef _WIN32
#include <syslog.h>
#endif // _WIN32

namespace _SMERP {

	class ConsoleLogBackend
	{
	public:
		ConsoleLogBackend( ) { logLevel_ = _SMERP::LogLevel::LOGLEVEL_NOTICE; }
		~ConsoleLogBackend( ) { }

		void setLevel( const LogLevel::Level level )	{
			logLevel_ = level;
		}
		
		inline void log( const LogLevel::Level level, const std::string& msg )	{
			if ( level >= logLevel_ )
				std::cerr << level << ": " << msg << std::endl;
		}
	private:
		LogLevel::Level	logLevel_;		
	};

#ifndef _WIN32
	class SyslogBackend
	{
	public:
		SyslogBackend( ) {
			logLevel_ = _SMERP::LogLevel::LOGLEVEL_ERROR;
			facility_ = facilityToSyslogFacility( _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_DAEMON );
			ident_ = "SMERP";
			openlog( ident_.c_str( ), LOG_CONS | LOG_PID | LOG_NDELAY, facility_ );
		}
		
		~SyslogBackend( ) {
			closelog( );
		}
		
		void setLevel( const LogLevel::Level level )	{
			logLevel_ = level;
		}
		
		void setFacility( const SyslogFacility::Facility facility ) {
			facility_ = facility;
		}
		
		void setIdent( const std::string &ident ) {
			ident_ = ident;
		}
		
		inline void log( const LogLevel::Level level, const std::string& msg )	{
			if ( level >= logLevel_ )
				syslog( levelToSyslogLevel( level ), "%s", msg.c_str( ) );
		}

	private:
		LogLevel::Level logLevel_;
		int facility_;
		std::string ident_;

		int levelToSyslogLevel( const LogLevel::Level level );
		int facilityToSyslogFacility( const SyslogFacility::Facility );
	};
#endif // _WIN32	

	class LogBackend
	{
	public:
		LogBackend() { }
		
		void setConsoleLevel( const LogLevel::Level level )	{
			consoleLogger_.setLevel( level );
		}

#ifndef _WIN32		
		void setSyslogLevel( const LogLevel::Level level )	{
			syslogLogger_.setLevel( level );
		}
#endif //_WIN32
			
		~LogBackend()	{ /* free logger resources here */ }
		
		inline void log( const LogLevel::Level level, const std::string& msg )	{
			consoleLogger_.log( level, msg );
#ifndef _WIN32
			syslogLogger_.log( level, msg );
#endif // _WIN32
		}
	private:
		ConsoleLogBackend consoleLogger_;
#ifndef _WIN32
		SyslogBackend syslogLogger_;
#endif // _WIN32	
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

#ifndef NO_LOG_MACROS

#ifndef _WIN32
#undef LOG_DEBUG
#undef LOG_INFO
#undef LOG_NOTICE
#undef LOG_WARNING
#undef LOG_ALERT
#endif // _WIN32

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

#endif // NO_LOG_MACROS

#endif // _LOGGER_HPP_INCLUDED
