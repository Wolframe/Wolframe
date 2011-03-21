//
// logBackend.hpp
//

#ifndef _LOG_BACKEND_HPP_INCLUDED
#define _LOG_BACKEND_HPP_INCLUDED

#include "appConfig.hpp"
#include "logLevel.hpp"
#include "logSyslogFacility.hpp"

#include <string>
#include <fstream>
#include <sstream>

#if defined( _WIN32 )
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif // defined( _WIN32 )

namespace _Wolframe {
	namespace Logging {

	/// Logger backends
	class ConsoleLogBackend
	{
	public:
		ConsoleLogBackend( );

		~ConsoleLogBackend( );

		void setLevel( const LogLevel::Level level );

		void log( const LogComponent component, const LogLevel::Level level, const std::string& msg );

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

		void log( const LogComponent component, const LogLevel::Level level, const std::string& msg );

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

		void log( const LogComponent component, const LogLevel::Level level, const std::string& msg );

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

		void log( const LogComponent component, const LogLevel::Level level, const std::string& msg );

		void reopen( );

	private:
		LogLevel::Level logLevel_;
		HANDLE eventSource_;
		std::string log_;
		std::string source_;
		PSID sid_;

		void calculateSid( );		
	};
#endif // _WIN32

	class LogBackend::LogBackendImpl
	{
	public:
		LogBackendImpl( );

		~LogBackendImpl( );

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

		void log( const LogComponent component, const LogLevel::Level level, const std::string& msg );

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

	} // namespace Logging
} // namespace _Wolframe

#endif // _LOG_BACKEND_HPP_INCLUDED
