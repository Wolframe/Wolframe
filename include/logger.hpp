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


namespace _Wolframe {

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

} // namespace _Wolframe


// shortcut macros
#define LOG_DATA	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NONE, _Wolframe::LogLevel::LOGLEVEL_DATA )
#define LOG_TRACE	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NONE, _Wolframe::LogLevel::LOGLEVEL_TRACE )
#define LOG_DEBUG	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NONE, _Wolframe::LogLevel::LOGLEVEL_DEBUG )
#define LOG_INFO	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NONE, _Wolframe::LogLevel::LOGLEVEL_INFO )
#define LOG_NOTICE	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NONE, _Wolframe::LogLevel::LOGLEVEL_NOTICE )
#define LOG_WARNING	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NONE, _Wolframe::LogLevel::LOGLEVEL_WARNING )
#define LOG_ERROR	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NONE, _Wolframe::LogLevel::LOGLEVEL_ERROR )
#define LOG_SEVERE	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NONE, _Wolframe::LogLevel::LOGLEVEL_SEVERE )
#define LOG_CRITICAL	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NONE, _Wolframe::LogLevel::LOGLEVEL_CRITICAL )
#define LOG_ALERT	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NONE, _Wolframe::LogLevel::LOGLEVEL_ALERT )
#define LOG_FATAL	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NONE, _Wolframe::LogLevel::LOGLEVEL_FATAL )

#define LOG_NETWORK_DATA	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::LogLevel::LOGLEVEL_DATA )
#define LOG_NETWORK_TRACE	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::LogLevel::LOGLEVEL_TRACE )
#define LOG_NETWORK_DEBUG	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::LogLevel::LOGLEVEL_DEBUG )
#define LOG_NETWORK_INFO	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::LogLevel::LOGLEVEL_INFO )
#define LOG_NETWORK_NOTICE	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::LogLevel::LOGLEVEL_NOTICE )
#define LOG_NETWORK_WARNING	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::LogLevel::LOGLEVEL_WARNING )
#define LOG_NETWORK_ERROR	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::LogLevel::LOGLEVEL_ERROR )
#define LOG_NETWORK_SEVERE	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::LogLevel::LOGLEVEL_SEVERE )
#define LOG_NETWORK_CRITICAL	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::LogLevel::LOGLEVEL_CRITICAL )
#define LOG_NETWORK_ALERT	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::LogLevel::LOGLEVEL_ALERT )
#define LOG_NETWORK_FATAL	_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get( _Wolframe::LogComponent::LOGCOMPONENT_NETWORK, _Wolframe::LogLevel::LOGLEVEL_FATAL )

#endif // _LOGGER_HPP_INCLUDED
