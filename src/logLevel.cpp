//
// logLevel.cpp
//

#include "logLevel.hpp"

#include <string>
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
	namespace Logging {

	LogLevel::Level LogLevel::str2LogLevel( const std::string str )
	{
		std::string s = boost::algorithm::to_upper_copy( str );
		boost::algorithm::trim( s );
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

	} // namespace Logging
} // namespace _Wolframe
