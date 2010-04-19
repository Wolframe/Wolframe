//
// logSyslogFacility.cpp
//

#include "logSyslogFacility.hpp"

#include <boost/algorithm/string.hpp>

namespace _SMERP {


SyslogFacility::Facility SyslogFacility::str2SyslogFacility( const std::string str )
{
	std::string s = str;
	boost::trim( s );
	boost::to_upper( s );

	if( s == "KERN" )		return SyslogFacility::_SMERP_SYSLOG_FACILITY_KERN;
	else if( s == "USER" )		return SyslogFacility::_SMERP_SYSLOG_FACILITY_USER;
	else if( s == "MAIL" )		return SyslogFacility::_SMERP_SYSLOG_FACILITY_MAIL;
	else if( s == "DAEMON" )	return SyslogFacility::_SMERP_SYSLOG_FACILITY_DAEMON;
	else if( s == "AUTH" )		return SyslogFacility::_SMERP_SYSLOG_FACILITY_AUTH;
	else if( s == "SYSLOG" )	return SyslogFacility::_SMERP_SYSLOG_FACILITY_SYSLOG;
	else if( s == "LPR" )		return SyslogFacility::_SMERP_SYSLOG_FACILITY_LPR;
	else if( s == "NEWS" )		return SyslogFacility::_SMERP_SYSLOG_FACILITY_NEWS;
	else if( s == "UUCP" )		return SyslogFacility::_SMERP_SYSLOG_FACILITY_UUCP;
	else if( s == "CRON" )		return SyslogFacility::_SMERP_SYSLOG_FACILITY_CRON;
	else if( s == "AUTHPRIV" )	return SyslogFacility::_SMERP_SYSLOG_FACILITY_AUTHPRIV;
	else if( s == "FTP" )		return SyslogFacility::_SMERP_SYSLOG_FACILITY_FTP;
	else if( s == "NTP" )		return SyslogFacility::_SMERP_SYSLOG_FACILITY_NTP;
	else if( s == "SECURITY" )	return SyslogFacility::_SMERP_SYSLOG_FACILITY_SECURITY;
	else if( s == "LOCAL0" )	return SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL0;
	else if( s == "LOCAL1" )	return SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL1;
	else if( s == "LOCAL2" )	return SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL2;
	else if( s == "LOCAL3" )	return SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL3;
	else if( s == "LOCAL4" )	return SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL4;
	else if( s == "LOCAL5" )	return SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL5;
	else if( s == "LOCAL6" )	return SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL6;
	else if( s == "LOCAL7" )	return SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL7;
	else				return SyslogFacility::_SMERP_SYSLOG_FACILITY_UNDEFINED;
}

} // namespace _SMERP
