//
// logger.hpp
//

#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

#include "logLevel.hpp"
#include "appConfig.hpp"
#include "singleton.hpp"

#include <string>
#include <fstream>
#include <sstream>

#if defined( _WIN32 )
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif // defined( _WIN32 )

namespace _SMERP {

	class ConsoleLogBackend
	{
	public:
		ConsoleLogBackend( );

		~ConsoleLogBackend( );

		void setLevel( const LogLevel::Level level );

		void log( const LogLevel::Level level, const std::string& msg );

		void reopen( );

	private:
		LogLevel::Level	logLevel_;
	};

	class LogfileBackend
	{
	public:
		LogfileBackend( );

		~LogfileBackend( );

		void setLevel( const LogLevel::Level level );

		void setFilename( const std::string filename );

		void reopen( );

		void log( const LogLevel::Level level, const std::string& msg );

	private:
		LogLevel::Level logLevel_;
		std::ofstream logFile_;
		std::string filename_;
		bool isOpen_;
	};

#ifndef _WIN32
	class SyslogBackend
	{
	public:
		SyslogBackend( );

		~SyslogBackend( );

		void setLevel( const LogLevel::Level level );

		void setFacility( const SyslogFacility::Facility facility );

		void setIdent( const std::string ident );

		void log( const LogLevel::Level level, const std::string& msg );

		void reopen( );

	private:
		LogLevel::Level logLevel_;
		int facility_;
		std::string ident_;
	};
#endif // _WIN32

#ifdef _WIN32
	class EventlogBackend
	{
	public:
		EventlogBackend( );

		~EventlogBackend( );

		void setLevel( const LogLevel::Level level );

		void setLog( const std::string log );

		void setSource( const std::string source );

		void log( const LogLevel::Level level, const std::string& msg );

		void reopen( );

	private:
		LogLevel::Level logLevel_;
		DWORD categoryId_;
		HANDLE eventSource_;
		std::string log_;
		std::string source_;
	};
#endif // _WIN32

	class LogBackend
	{
	public:
		LogBackend( );

		~LogBackend( );

		void setConsoleLevel( const LogLevel::Level level );

		void setLogfileLevel( const LogLevel::Level level );

		void setLogfileName( const std::string filename );

#ifndef _WIN32
		void setSyslogLevel( const LogLevel::Level level );

		void setSyslogFacility( const SyslogFacility::Facility facility );

		void setSyslogIdent( const std::string ident );
#endif // _WIN32

#ifdef _WIN32
		void setEventlogLevel( const LogLevel::Level level );

		void setEventlogLog( const std::string log );

		void setEventlogSource( const std::string source );
#endif // _WIN32

		void log( const LogLevel::Level level, const std::string& msg );

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

//	class Logger {
//	public:
//		Logger( LogBackend& backend );

//		~Logger( );

//		std::ostringstream& Get( LogLevel::Level level );

//	protected:
//		std::ostringstream os_;

//	private:
//		LogBackend&	logBk_;
//		LogLevel::Level	msgLevel_;

//		Logger( );
//		Logger( const Logger& );
//		Logger& operator= ( const Logger& );
//	};

	class Logger:: {
	public:
		Logger( LogBackend& backend );

		~Logger( );

		std::ostringstream& Get( LogLevel::Level level );

	protected:
		std::ostringstream os_;

	private:
		LogBackend&	logBk_;
		LogLevel::Level	msgLevel_;

		Logger( );
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
