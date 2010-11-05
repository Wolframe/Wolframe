//
// syslogFacility.hpp
//

#ifndef _LOG_SYSLOG_FACILITY_HPP_INCLUDED
#define _LOG_SYSLOG_FACILITY_HPP_INCLUDED

#include <string>
#include <iostream>

namespace _SMERP {

class SyslogFacility {
public:
	enum Facility	{
		FACILITY_KERN,		/// kernel messages
		FACILITY_USER,		/// random user-level messages
		FACILITY_MAIL,		/// mail system
		FACILITY_DAEMON,	/// system daemons
		FACILITY_AUTH,		/// security/authorization messages
		FACILITY_SYSLOG,	/// messages generated internally by syslog
		FACILITY_LPR,		/// line printer subsystem
		FACILITY_NEWS,		/// network news subsystem
		FACILITY_UUCP,		/// UUCP subsystem
		FACILITY_CRON,		/// cron/at daemons
		FACILITY_AUTHPRIV,	/// security/authorization messages
		FACILITY_FTP,		/// FTP daemon
		FACILITY_NTP,		/// NTP daemon (on FreeBSD)
		FACILITY_SECURITY,	/// security subsystem (on FreeBSD)
		FACILITY_LOCAL0,	/// local facility 0
		FACILITY_LOCAL1,	/// local facility 1
		FACILITY_LOCAL2,	/// local facility 2
		FACILITY_LOCAL3,	/// local facility 3
		FACILITY_LOCAL4,	/// local facility 4
		FACILITY_LOCAL5,	/// local facility 5
		FACILITY_LOCAL6,	/// local facility 6
		FACILITY_LOCAL7,	/// local facility 7
		FACILITY_UNDEFINED	/// placeholder
	};

	static Facility str2SyslogFacility( const std::string s );
	static int facility2int( const Facility f );
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
