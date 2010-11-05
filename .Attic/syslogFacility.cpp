//
// syslogFacility.cpp
//

#include "syslogFacility.hpp"
#include <syslog.h>
#include <boost/algorithm/string.hpp>

namespace _dR {


SyslogFacility::Facility SyslogFacility::str2SyslogFacility( const std::string str )
{
	std::string s = str;
	boost::trim( s );
	boost::to_upper( s );

	if( s == "KERN" )		return SyslogFacility::FACILITY_KERN;
	else if( s == "USER" )		return SyslogFacility::FACILITY_USER;
	else if( s == "MAIL" )		return SyslogFacility::FACILITY_MAIL;
	else if( s == "DAEMON" )	return SyslogFacility::FACILITY_DAEMON;
	else if( s == "AUTH" )		return SyslogFacility::FACILITY_AUTH;
	else if( s == "SYSLOG" )	return SyslogFacility::FACILITY_SYSLOG;
	else if( s == "LPR" )		return SyslogFacility::FACILITY_LPR;
	else if( s == "NEWS" )		return SyslogFacility::FACILITY_NEWS;
	else if( s == "UUCP" )		return SyslogFacility::FACILITY_UUCP;
	else if( s == "CRON" )		return SyslogFacility::FACILITY_CRON;
	else if( s == "AUTHPRIV" )	return SyslogFacility::FACILITY_AUTHPRIV;
	else if( s == "FTP" )		return SyslogFacility::FACILITY_FTP;
	else if( s == "NTP" )		return SyslogFacility::FACILITY_NTP;
	else if( s == "SECURITY" )	return SyslogFacility::FACILITY_SECURITY;
	else if( s == "LOCAL0" )	return SyslogFacility::FACILITY_LOCAL0;
	else if( s == "LOCAL1" )	return SyslogFacility::FACILITY_LOCAL1;
	else if( s == "LOCAL2" )	return SyslogFacility::FACILITY_LOCAL2;
	else if( s == "LOCAL3" )	return SyslogFacility::FACILITY_LOCAL3;
	else if( s == "LOCAL4" )	return SyslogFacility::FACILITY_LOCAL4;
	else if( s == "LOCAL5" )	return SyslogFacility::FACILITY_LOCAL5;
	else if( s == "LOCAL6" )	return SyslogFacility::FACILITY_LOCAL6;
	else if( s == "LOCAL7" )	return SyslogFacility::FACILITY_LOCAL7;
	else				return SyslogFacility::FACILITY_UNDEFINED;
}


int SyslogFacility::facility2int( const SyslogFacility::Facility f )
{
	switch( f )	{
		case FACILITY_KERN:	return LOG_KERN;
		case FACILITY_USER:	return LOG_USER;
		case FACILITY_MAIL:	return LOG_MAIL;
		case FACILITY_DAEMON:	return LOG_DAEMON;
		case FACILITY_AUTH:	return LOG_AUTHPRIV;
		case FACILITY_SYSLOG:	return LOG_SYSLOG;
		case FACILITY_LPR:	return LOG_LPR;
		case FACILITY_NEWS:	return LOG_NEWS;
		case FACILITY_UUCP:	return LOG_UUCP;
		case FACILITY_CRON:	return LOG_CRON;
		case FACILITY_AUTHPRIV:	return LOG_AUTHPRIV;
		case FACILITY_FTP:	return LOG_FTP;
		case FACILITY_NTP:	return LOG_DAEMON;
		case FACILITY_SECURITY:	return LOG_AUTHPRIV;
		case FACILITY_LOCAL0:	return LOG_LOCAL0;
		case FACILITY_LOCAL1:	return LOG_LOCAL1;
		case FACILITY_LOCAL2:	return LOG_LOCAL2;
		case FACILITY_LOCAL3:	return LOG_LOCAL3;
		case FACILITY_LOCAL4:	return LOG_LOCAL4;
		case FACILITY_LOCAL5:	return LOG_LOCAL5;
		case FACILITY_LOCAL6:	return LOG_LOCAL6;
		case FACILITY_LOCAL7:	return LOG_LOCAL7;

		case FACILITY_UNDEFINED:
		default:
			return LOG_USER;
	}
}


} // namespace _dR
