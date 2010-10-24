//
// logLevel.cpp
//

#include "logLevel.hpp"

#include <boost/algorithm/string.hpp>

namespace _SMERP {


LogLevel::Level LogLevel::str2LogLevel( const std::string str )
{
	std::string s = str;
	boost::trim( s );
	boost::to_upper( s );

	if( s == "DATA" )		return LogLevel::LOG_DATA;
	else if( s == "TRACE" )		return LogLevel::LOG_TRACE;
	else if( s == "DEBUG" )		return LogLevel::LOG_DEBUG;
	else if( s == "INFO" )		return LogLevel::LOG_INFO;
	else if( s == "NOTICE" )	return LogLevel::LOG_NOTICE;
	else if( s == "WARNING" )	return LogLevel::LOG_WARNING;
	else if( s == "ERROR" )		return LogLevel::LOG_ERROR;
	else if( s == "SEVERE" )	return LogLevel::LOG_SEVERE;
	else if( s == "CRITICAL" )	return LogLevel::LOG_CRITICAL;
	else if( s == "ALERT" )		return LogLevel::LOG_ALERT;
	else if( s == "FATAL" )		return LogLevel::LOG_FATAL;
	else				return LogLevel::LOG_UNDEFINED;
}

} // namespace _SMERP
