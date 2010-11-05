//
// logLevel.cpp
//

#include "logLevel.hpp"

namespace _SMERP {


LogLevel::Level LogLevel::str2LogLevel( const std::string s )
{
	if( s == "DATA" )		return LogLevel::LOGLEVEL_DATA;
	else if( s == "TRACE" )		return LogLevel::LOGLEVEL_TRACE;
	else if( s == "DEBUG" )		return LogLevel::LOGLEVEL_DEBUG;
	else if( s == "INFO" )		return LogLevel::LOGLEVEL_INFO;
	else if( s == "NOTICE" )	return LogLevel::LOGLEVEL_NOTICE;
	else if( s == "WARNING" )	return LogLevel::LOGLEVEL_WARNING;
	else if( s == "ERROR" )		return LogLevel::LOGLEVEL_ERROR;
	else if( s == "SEVERE" )	return LogLevel::LOGLEVEL_SEVERE;
	else if( s == "CRITICAL" )	return LogLevel::LOGLEVEL_CRITICAL;
	else if( s == "ALERT" )		return LogLevel::LOGLEVEL_ALERT;
	else if( s == "FATAL" )		return LogLevel::LOGLEVEL_FATAL;
	else				return LogLevel::LOGLEVEL_UNDEFINED;
}

} // namespace _SMERP
