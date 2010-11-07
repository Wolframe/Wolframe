//
// logger.cpp
//

#define NO_LOG_MACROS
#include "logger.hpp"

#ifndef _WIN32
#include <syslog.h>
#endif // _WIN32

namespace _SMERP {

#ifndef _WIN32
	int SyslogBackend::levelToSyslogLevel( const LogLevel::Level level )	{
		switch( level )	{
			case _SMERP::LogLevel::LOGLEVEL_DATA:
			case _SMERP::LogLevel::LOGLEVEL_TRACE:
			case _SMERP::LogLevel::LOGLEVEL_DEBUG:		return LOG_DEBUG;
			case _SMERP::LogLevel::LOGLEVEL_INFO:		return LOG_INFO;
			case _SMERP::LogLevel::LOGLEVEL_NOTICE:		return LOG_NOTICE;
			case _SMERP::LogLevel::LOGLEVEL_WARNING:	return LOG_WARNING;
			case _SMERP::LogLevel::LOGLEVEL_ERROR:		return LOG_ERR;
			case _SMERP::LogLevel::LOGLEVEL_SEVERE:
			case _SMERP::LogLevel::LOGLEVEL_CRITICAL:	return LOG_CRIT;
			case _SMERP::LogLevel::LOGLEVEL_ALERT:		return LOG_ALERT;
			case _SMERP::LogLevel::LOGLEVEL_FATAL:		return LOG_EMERG;
			case _SMERP::LogLevel::LOGLEVEL_UNDEFINED:	return LOG_ERR;
		}
		return LOG_ERR;
	}
#endif // _WIN32

} // namespace _SMERP
