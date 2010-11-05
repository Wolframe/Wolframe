//
// syslogLogger.hpp
//

#ifndef _SYSLOG_LOGGER_H_INCLUDED
#define _SYSLOG_LOGGER_H_INCLUDED

#include <string>
#include "syslogFacility.hpp"
#include "logLevel.hpp"

namespace _SMERP {

class syslogLogger
{
public:
	syslogLogger( std::string ident, SyslogFacility::Facility f, LogLevel::Level l = LogLevel::_SMERP_INFO );
	~syslogLogger( void );

	void level( LogLevel::Level l );
	LogLevel::Level level()				{ return level_; }

	void log( LogLevel::Level l, const std::string& msg );
	void reopen();

private:
	SyslogFacility::Facility	facility_;
	LogLevel::Level			level_;
	std::string			ident_;
};


} // namespace _SMERP

#endif // _SYSLOG_LOGGER_H_INCLUDED
