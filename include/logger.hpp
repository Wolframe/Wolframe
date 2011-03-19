//
// logger.hpp
//

#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

#ifdef _WIN32
#include <tchar.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <strsafe.h>
#else
#include <errno.h>
#include <cstring>
#endif

#include "singleton.hpp"
#include "logLevel.hpp"
#include "logSyslogFacility.hpp"

#include <string>
#include <fstream>
#include <sstream>


#include "unused.h"

namespace _Wolframe {
	namespace Logging {

	class LogComponent
	{
	public:
		enum Component {
			LOGCOMPONENT_NONE,		/// no loging component
			LOGCOMPONENT_LOGGING,		/// internal logger errors
			LOGCOMPONENT_NETWORK,		/// networking
			LOGCOMPONENT_AUTH,		/// authentication
			LOGCOMPONENT_LUA,		/// lua processor
			LOGCOMPONENT_DUMMY
		};
		
	private:
		enum Component _component;

	public:			
		bool operator==( const LogComponent& o ) const {
			return _component == o._component;
		}
		
		LogComponent( const enum Component& c = LOGCOMPONENT_NONE ) : _component( c ) { }
		
		const char* str( ) const {
			static const char *const s[] = {
				"", "Logging", "Lua", "Network", "Auth" };
			if( static_cast< size_t >( _component ) < ( sizeof( s ) / sizeof( *s ) ) ) {
				return s[_component];
			} else {
				return "";
			}
		}

		static const LogComponent LogNone;
		static const LogComponent LogLogging;
		static const LogComponent LogLua;
		static const LogComponent LogNetwork;
		static const LogComponent LogAuth;			
	};

	class LogBackend : public Singleton< LogBackend >
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

		void log( const LogComponent component, const LogLevel::Level level, const std::string& msg );

	private:
		class LogBackendImpl;
		LogBackendImpl	*impl_;
	};

	class Logger {
	public:
		Logger( LogBackend& backend );

		~Logger( );

		Logger& Get( LogLevel::Level level );

		// OS error logging markers
		typedef struct { int _dummy; } LogStrerrorT;
		typedef struct LogWinerrorT { int _dummy; } LogWinerrorT;
		static const LogStrerrorT LogStrerror;
		static const LogWinerrorT LogWinerror;
				
		template<typename T> friend Logger& operator<<( Logger& logger, T thing );
		friend Logger& operator<<( Logger& logger, LogComponent thing );
		friend Logger& operator<<( Logger& logger, Logger::LogStrerrorT t );
		
	protected:
		std::ostringstream os_;

	private:
		LogBackend&	logBk_;
		LogLevel::Level	msgLevel_;
		LogComponent	component_;

		Logger( );
		Logger( const Logger& );
		Logger& operator= ( const Logger& );
	};

	const Logger::LogStrerrorT LogStrerror = { 1 };
	const Logger::LogWinerrorT LogWinerror = { 2 };
	
	// template functions for logging, default is we search for the << operator
	// and log with this one..
	template<typename T>
	Logger& operator<<( Logger& logger, T t )
	{
		logger.os_ << t;
		return logger;
	}

#ifdef _WIN32

// TODO
	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT > &operator<< ( 	std::basic_ostream< CharT, TraitsT >& os,
									const WOLFRAME_UNUSED _Wolframe::Logging::Logger::LogWinerrorT s )
	{
		DWORD last_error = GetLastError( );
		TCHAR errbuf[512];
		LPVOID werrbuf;
		DWORD wbuf_size;
		DWORD wres;
		
		wres = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS |
			FORMAT_MESSAGE_MAX_WIDTH_MASK,
			NULL,			// message is from system
			last_error,		// code of last error (GetLastError)
			0,			// default language (TODO: fit to i18n of rest)
			(LPTSTR)&werrbuf,	// use LocalAlloc for the message string
			0,			// minimal allocation size
			NULL );			// no arguments to the message
			
		if( wres == 0 ) {
			StringCbCopy( errbuf, 512, _T( "No message available" ) );
		}
	
		StringCbCopy( errbuf, 512, (LPCTSTR)werrbuf );
		
		os << errbuf;
		
		return os;
	}

#endif // defined( _WIN32 )
			
	} // namespace Logging
} // namespace _Wolframe

// shortcut macros
#define LOG_DATA	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_DATA )
#define LOG_TRACE	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_TRACE )
#define LOG_DEBUG	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_DEBUG )
#define LOG_INFO	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_INFO )
#define LOG_NOTICE	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_NOTICE )
#define LOG_WARNING	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_WARNING )
#define LOG_ERROR	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_ERROR )
#define LOG_SEVERE	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_SEVERE )
#define LOG_CRITICAL	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_CRITICAL )
#define LOG_ALERT	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_ALERT )
#define LOG_FATAL	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_FATAL )

#endif // _LOGGER_HPP_INCLUDED
