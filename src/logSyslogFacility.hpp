//
// logSyslogFacility.hpp
//

#ifndef _LOG_SYSLOG_FACILITY_HPP_INCLUDED
#define _LOG_SYSLOG_FACILITY_HPP_INCLUDED

#include <string>
#include <iostream>

namespace _SMERP {

class SyslogFacility {
public:
	enum Facility	{
		_SMERP_SYSLOG_FACILITY_KERN,		/// kernel messages
		_SMERP_SYSLOG_FACILITY_USER,		/// random user-level messages
		_SMERP_SYSLOG_FACILITY_MAIL,		/// mail system
		_SMERP_SYSLOG_FACILITY_DAEMON,		/// system daemons
		_SMERP_SYSLOG_FACILITY_AUTH,		/// security/authorization messages
		_SMERP_SYSLOG_FACILITY_SYSLOG,		/// messages generated internally by syslog
		_SMERP_SYSLOG_FACILITY_LPR,		/// line printer subsystem
		_SMERP_SYSLOG_FACILITY_NEWS,		/// network news subsystem
		_SMERP_SYSLOG_FACILITY_UUCP,		/// UUCP subsystem
		_SMERP_SYSLOG_FACILITY_CRON,		/// cron/at daemons
		_SMERP_SYSLOG_FACILITY_AUTHPRIV,	/// security/authorization messages
		_SMERP_SYSLOG_FACILITY_FTP,		/// FTP daemon
		_SMERP_SYSLOG_FACILITY_NTP,		/// NTP daemon (on FreeBSD)
		_SMERP_SYSLOG_FACILITY_SECURITY,	/// security subsystem (on FreeBSD)
		_SMERP_SYSLOG_FACILITY_LOCAL0,		/// local facility 0
		_SMERP_SYSLOG_FACILITY_LOCAL1,		/// local facility 1
		_SMERP_SYSLOG_FACILITY_LOCAL2,		/// local facility 2
		_SMERP_SYSLOG_FACILITY_LOCAL3,		/// local facility 3
		_SMERP_SYSLOG_FACILITY_LOCAL4,		/// local facility 4
		_SMERP_SYSLOG_FACILITY_LOCAL5,		/// local facility 5
		_SMERP_SYSLOG_FACILITY_LOCAL6,		/// local facility 6
		_SMERP_SYSLOG_FACILITY_LOCAL7,		/// local facility 7
		_SMERP_SYSLOG_FACILITY_UNDEFINED	/// placeholder
	};

	static Facility str2SyslogFacility( const std::string s );
};

// map enum values to strings
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT > &operator<< ( std::basic_ostream< CharT, TraitsT >& s, SyslogFacility::Facility f )
{
	static const char *const str[] = {
		"KERN", "USER", "MAIL", "DAEMON", "AUTH", "SYSLOG", "LPR",
		"NEWS", "UUCP", "CRON", "AUTHPRIV", "FTP", "NTP", "SECURITY",
		"LOCAL0", "LOCAL1", "LOCAL2", "LOCAL3", "LOCAL4", "LOCAL5",
		"LOCAL6", "LOCAL7" };
	if( static_cast< size_t >( f ) < ( sizeof( str ) / sizeof( *str ) ) ) {
		s << str[f];
	} else {
		s << "Unknown enum used '" << static_cast< int >( f ) << "'";
	}
	return s;
}

} // namespace _SMERP


#endif // _LOG_SYSLOG_FACILITY_HPP_INCLUDED
