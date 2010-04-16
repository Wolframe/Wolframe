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

	if( s == "DATA" )		return LogLevel::_SMERP_DATA;
	else if( s == "TRACE" )		return LogLevel::_SMERP_TRACE;
	else if( s == "DEBUG" )		return LogLevel::_SMERP_DEBUG;
	else if( s == "INFO" )		return LogLevel::_SMERP_INFO;
	else if( s == "NOTICE" )	return LogLevel::_SMERP_NOTICE;
	else if( s == "WARNING" )	return LogLevel::_SMERP_WARNING;
	else if( s == "ERROR" )		return LogLevel::_SMERP_ERROR;
	else if( s == "SEVERE" )	return LogLevel::_SMERP_SEVERE;
	else if( s == "CRITICAL" )	return LogLevel::_SMERP_CRITICAL;
	else if( s == "ALERT" )		return LogLevel::_SMERP_ALERT;
	else if( s == "FATAL" )		return LogLevel::_SMERP_FATAL;
	else				return LogLevel::_SMERP_UNDEFINED;
}

} // namespace _SMERP
