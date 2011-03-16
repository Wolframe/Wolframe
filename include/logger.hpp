//
// logger.hpp
//

#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

#include "singleton.hpp"
#include "logLevel.hpp"
#include "logSyslogFacility.hpp"
#include "logComponent.hpp"

#include <string>
#include <fstream>
#include <sstream>

#include "unused.h"

namespace _Wolframe {
	namespace Logging {

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

		void log( const LogComponent::Component component, const LogLevel::Level level, const std::string& msg );

	private:
		class LogBackendImpl;
		LogBackendImpl	*impl_;
	};

	class Logger {
	public:
		Logger( LogBackend& backend );

		~Logger( );

		std::ostringstream& Get( LogComponent::Component component_, LogLevel::Level level );

		typedef struct { int _dummy; } LogStrerrorT;
		static const LogStrerrorT LogStrerror;
		typedef struct { int _dummy; } LogWinerrorT;
		static const LogWinerrorT LogWinerror;
		
	protected:
		std::ostringstream os_;

	private:
		LogBackend&	logBk_;
		LogComponent::Component component_;
		LogLevel::Level	msgLevel_;

		Logger( );
		Logger( const Logger& );
		Logger& operator= ( const Logger& );
	};
		
	} // namespace Logging
} // namespace _Wolframe

// template functions for error markers in the output stream
// e.g. LOG_ERROR << "f() had a booboo, reason: " << Logger::LogStrerrorT

template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT > &operator<< ( 	std::basic_ostream< CharT, TraitsT >& os,
								const WOLFRAME_UNUSED _Wolframe::Logging::Logger::LogStrerrorT s ) {
	os << "MARKER";
	return os;
}

template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT > &operator<< ( 	std::basic_ostream< CharT, TraitsT >& os,
								const WOLFRAME_UNUSED _Wolframe::Logging::Logger::LogWinerrorT s ) {
	os << "MARKER";
	return os;
}

// shortcut macros
#define LOG_DATA	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NONE, _Wolframe::Logging::LogLevel::LOGLEVEL_DATA )
#define LOG_TRACE	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NONE, _Wolframe::Logging::LogLevel::LOGLEVEL_TRACE )
#define LOG_DEBUG	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NONE, _Wolframe::Logging::LogLevel::LOGLEVEL_DEBUG )
#define LOG_INFO	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NONE, _Wolframe::Logging::LogLevel::LOGLEVEL_INFO )
#define LOG_NOTICE	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NONE, _Wolframe::Logging::LogLevel::LOGLEVEL_NOTICE )
#define LOG_WARNING	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NONE, _Wolframe::Logging::LogLevel::LOGLEVEL_WARNING )
#define LOG_ERROR	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NONE, _Wolframe::Logging::LogLevel::LOGLEVEL_ERROR )
#define LOG_SEVERE	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NONE, _Wolframe::Logging::LogLevel::LOGLEVEL_SEVERE )
#define LOG_CRITICAL	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NONE, _Wolframe::Logging::LogLevel::LOGLEVEL_CRITICAL )
#define LOG_ALERT	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NONE, _Wolframe::Logging::LogLevel::LOGLEVEL_ALERT )
#define LOG_FATAL	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NONE, _Wolframe::Logging::LogLevel::LOGLEVEL_FATAL )

#define LOG_NETWORK_DATA	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::Logging::LogLevel::LOGLEVEL_DATA )
#define LOG_NETWORK_TRACE	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::Logging::LogLevel::LOGLEVEL_TRACE )
#define LOG_NETWORK_DEBUG	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::Logging::LogLevel::LOGLEVEL_DEBUG )
#define LOG_NETWORK_INFO	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::Logging::LogLevel::LOGLEVEL_INFO )
#define LOG_NETWORK_NOTICE	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::Logging::LogLevel::LOGLEVEL_NOTICE )
#define LOG_NETWORK_WARNING	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::Logging::LogLevel::LOGLEVEL_WARNING )
#define LOG_NETWORK_ERROR	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::Logging::LogLevel::LOGLEVEL_ERROR )
#define LOG_NETWORK_SEVERE	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::Logging::LogLevel::LOGLEVEL_SEVERE )
#define LOG_NETWORK_CRITICAL	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::Logging::LogLevel::LOGLEVEL_CRITICAL )
#define LOG_NETWORK_ALERT	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::Logging::LogLevel::LOGLEVEL_ALERT )
#define LOG_NETWORK_FATAL	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::Logging::LogLevel::LOGLEVEL_FATAL )

#endif // _LOGGER_HPP_INCLUDED
