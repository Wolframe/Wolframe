//
// logger.hpp
//

#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

#include "logLevel.hpp"
#include "appConfig.hpp"

#include <ostream>
#include <fstream>

#if !defined( _WIN32 )
#include <syslog.h>
#endif // !defined( _WIN32 )

#if defined( _WIN32 )
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif // defined( _WIN32 )

namespace _SMERP {

	class ConsoleLogBackend
	{
	public:
		ConsoleLogBackend( ) { logLevel_ = _SMERP::LogLevel::LOGLEVEL_ERROR; }
		~ConsoleLogBackend( ) { /* nothing to do here */ }

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
	
	class LogfileBackend
	{
	public:
		LogfileBackend( ) {
			logLevel_ = _SMERP::LogLevel::LOGLEVEL_UNDEFINED;
			// we don't open a primare unknown logfile, wait for setFilename
		}
		
		~LogfileBackend( ) {
			logFile_.close( );
		}
		
		void setLevel( const LogLevel::Level level )	{
			logLevel_ = level;
		}
		
		void setFilename( const std::string filename ) {
			filename_ = filename;
			reopen( );
		}

		void reopen( ) {
			logFile_.close( );
			logFile_.open( filename_.c_str( ), std::ios_base::out | std::ios_base::app );
		}	
		
		inline void log( const LogLevel::Level level, const std::string& msg )	{
			if( level >= logLevel_ ) {
				logFile_ << level << ": " << msg << std::endl;
				logFile_.flush( );
			}
		}	
		
	private:
		LogLevel::Level logLevel_;
		std::ofstream logFile_;
		std::string filename_;
	};

#ifndef _WIN32
	class SyslogBackend
	{
	public:
		SyslogBackend( ) {
			logLevel_ = _SMERP::LogLevel::LOGLEVEL_UNDEFINED;
			ident_ = "<undefined>";
			facility_ = _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_DAEMON;
			openlog( ident_.c_str( ), LOG_CONS | LOG_PID, facility_ );
		}
		
		~SyslogBackend( ) {
			closelog( );
		}
		
		void setLevel( const LogLevel::Level level )	{
			logLevel_ = level;
		}
		
		void setFacility( const SyslogFacility::Facility facility ) {
			facility_ = facilityToSyslogFacility( facility );
			reopen( );
		}
		
		void setIdent( const std::string ident ) {
			ident_ = ident;
			reopen( );
		}
		
		inline void log( const LogLevel::Level level, const std::string& msg )	{
			if ( level >= logLevel_ )
				syslog( levelToSyslogLevel( level ), "%s", msg.c_str( ) );
		}

		void reopen( ) {
			closelog( );
			openlog( ident_.c_str( ), LOG_CONS | LOG_PID, facility_ );
		}
		
	private:
		LogLevel::Level logLevel_;
		int facility_;
		std::string ident_;

		int levelToSyslogLevel( const LogLevel::Level level );
		int facilityToSyslogFacility( const SyslogFacility::Facility );		
	};
#endif // _WIN32

#ifdef _WIN32
	class EventlogBackend
	{
	public:
		EventlogBackend( ) {
			logLevel_ = _SMERP::LogLevel::LOGLEVEL_UNDEFINED;
			categoryId_ = 1 | 0x0FFF0000L; // the one category we have at the moment in the resource
			log_ = "Application";
			source_ = "<undefined>";
		        eventSource_ = RegisterEventSource( NULL, source_.c_str( ) );
		}
		
		~EventlogBackend( ) {
			if( eventSource_ ) {
				(void)DeregisterEventSource( eventSource_ );
				eventSource_ = 0;
			}
		}
		
		void setLevel( const LogLevel::Level level )	{
			logLevel_ = level;
		}

		void setLog( const std::string log ) {
			log_ = log;
			reopen( );
		}
		
		void setSource( const std::string source ) {
			source_ = source;
			reopen( );
		}
				
		inline void log( const LogLevel::Level level, const std::string& msg )	{
			if ( level >= logLevel_ ) {
				LPCSTR msg_arr[1];
				msg_arr[0] = (LPSTR)msg.c_str( );
				(void)ReportEvent(
					eventSource_,
					levelToEventlogLevel( level ),
					categoryId_, 
					messageIdToEventlogId( level ),
					NULL, // SID of the user owning the process, not now, later..
					1, // at the moment no strings to replace, just the message itself
					0, // no binary data
					msg_arr, // array of strings to log (msg.c_str() for now)
					NULL ); // no binary data
			}					
		}

		void reopen( ) {
			if( eventSource_ )
				(void)DeregisterEventSource( eventSource_ );
			eventSource_ = RegisterEventSource( NULL, source_.c_str( ) );
		}		
	
	private:
		LogLevel::Level logLevel_;
		DWORD categoryId_;
		HANDLE eventSource_;
		std::string log_;
		std::string source_;
		
		DWORD levelToEventlogLevel( const LogLevel::Level level );
		DWORD messageIdToEventlogId( DWORD eventLogLevel );
	};
#endif // _WIN32	

	class LogBackend
	{
	public:
		LogBackend() { }
		
		void setConsoleLevel( const LogLevel::Level level )	{
			consoleLogger_.setLevel( level );
		}
		
		void setLogfileLevel( const LogLevel::Level level )	{
			logfileLogger_.setLevel( level );
		}

		void setLogfileName( const std::string filename )	{
			logfileLogger_.setFilename( filename );
		}

#ifndef _WIN32		
		void setSyslogLevel( const LogLevel::Level level )	{
			syslogLogger_.setLevel( level );
		}
		
		void setSyslogFacility( const SyslogFacility::Facility facility )	{
			syslogLogger_.setFacility( facility );
		}
		
		void setSyslogIdent( const std::string ident )	{
			syslogLogger_.setIdent( ident );
		}
#endif // _WIN32

#ifdef _WIN32
		void setEventlogLevel( const LogLevel::Level level ) {
			eventlogLogger_.setLevel( level );
		}
		
		void setEventlogLog( const std::string log ) {
			eventlogLogger_.setLog( log );
		}
		
		void setEventlogSource( const std::string source ) {
			eventlogLogger_.setSource( source );
		}
#endif // _WIN32	
		~LogBackend()	{ /* free logger resources here */ }
		
		inline void log( const LogLevel::Level level, const std::string& msg )	{
			consoleLogger_.log( level, msg );
			logfileLogger_.log( level, msg );
#ifndef _WIN32
			syslogLogger_.log( level, msg );
#endif //
#ifdef _WIN32
			eventlogLogger_.log( level, msg );
#endif // _WIN32
		}
	private:
		ConsoleLogBackend consoleLogger_;
		LogfileBackend logfileLogger_;
#ifndef _WIN32
		SyslogBackend syslogLogger_;
#endif // _WIN32	
#ifdef _WIN32
		EventlogBackend eventlogLogger_;
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
