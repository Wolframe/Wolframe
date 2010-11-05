//
// syslogLogger.cpp
//

#include <cassert>
#include <string>
#include <syslog.h>
#include "logLevel.hpp"
#include "syslogFacility.hpp"
#include "syslogLogger.hpp"


namespace _SMERP {

syslogLogger::syslogLogger( std::string ident, SyslogFacility::Facility facility, LogLevel::Level l )
{
	assert ( LogLevel::isLevel( l ) );
	level_ = l;
	ident_ = ident;
	facility_ = facility;
	openlog( ident_.c_str(), LOG_CONS | LOG_PID, SyslogFacility::facility2int( facility_ ));
}

syslogLogger::~syslogLogger()
{
	closelog();
}


void syslogLogger::level( LogLevel::Level l )
{
	assert ( LogLevel::isLevel( l ) );
	level_ = l;
}


void syslogLogger::reopen()
{
	closelog();
	openlog( ident_.c_str(), LOG_CONS | LOG_PID, SyslogFacility::facility2int( facility_ ));
}


void syslogLogger::log( LogLevel::Level l, const std::string& msg )
{
	if ( l >= level_ )
		syslog( LogLevel::level2syslog( l ), "%s", msg.c_str());
}

} // namespace _SMERP
