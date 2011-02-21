//
// logger.hpp
//

#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

#include "singleton.hpp"
#include "logLevel.hpp"
#include "logSyslogFacility.hpp"

#include <string>
#include <fstream>
#include <sstream>


namespace _SMERP {

	class LogBackend : public Singleton< LogBackend >
	{
	public:
		LogBackend( );

		~LogBackend( );

		void setConsoleLevel( const LogLevel::Level level );
		
		void setConsolePrefix( const std::string prefix );

		void setLogfileLevel( const LogLevel::Level level );

		void setLogfileName( const std::string filename );

		void setLogfilePrefix( const std::string prefix );

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
		class LogBackendImpl;
		LogBackendImpl	*impl_;
	};

	class Logger {
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


// shortcut macros
#define LOG_DATA	_SMERP::Logger( _SMERP::LogBackend::instance() ).Get( _SMERP::LogLevel::LOGLEVEL_DATA )
#define LOG_TRACE	_SMERP::Logger( _SMERP::LogBackend::instance() ).Get( _SMERP::LogLevel::LOGLEVEL_TRACE )
#define LOG_DEBUG	_SMERP::Logger( _SMERP::LogBackend::instance() ).Get( _SMERP::LogLevel::LOGLEVEL_DEBUG )
#define LOG_INFO	_SMERP::Logger( _SMERP::LogBackend::instance() ).Get( _SMERP::LogLevel::LOGLEVEL_INFO )
#define LOG_NOTICE	_SMERP::Logger( _SMERP::LogBackend::instance() ).Get( _SMERP::LogLevel::LOGLEVEL_NOTICE )
#define LOG_WARNING	_SMERP::Logger( _SMERP::LogBackend::instance() ).Get( _SMERP::LogLevel::LOGLEVEL_WARNING )
#define LOG_ERROR	_SMERP::Logger( _SMERP::LogBackend::instance() ).Get( _SMERP::LogLevel::LOGLEVEL_ERROR )
#define LOG_SEVERE	_SMERP::Logger( _SMERP::LogBackend::instance() ).Get( _SMERP::LogLevel::LOGLEVEL_SEVERE )
#define LOG_CRITICAL	_SMERP::Logger( _SMERP::LogBackend::instance() ).Get( _SMERP::LogLevel::LOGLEVEL_CRITICAL )
#define LOG_ALERT	_SMERP::Logger( _SMERP::LogBackend::instance() ).Get( _SMERP::LogLevel::LOGLEVEL_ALERT )
#define LOG_FATAL	_SMERP::Logger( _SMERP::LogBackend::instance() ).Get( _SMERP::LogLevel::LOGLEVEL_FATAL )

#endif // _LOGGER_HPP_INCLUDED
