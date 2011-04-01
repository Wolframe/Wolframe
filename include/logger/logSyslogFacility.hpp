
///
/// \file logSyslogFacility.hpp
/// \brief Defines facilities of the system logger (Unix only)
///

#ifndef _LOG_SYSLOG_FACILITY_HPP_INCLUDED
#define _LOG_SYSLOG_FACILITY_HPP_INCLUDED

#include <string>
#include <iosfwd>

namespace _Wolframe {
	namespace Logging {

	class SyslogFacility {
	public:
		enum Facility	{
			WOLFRAME_SYSLOG_FACILITY_KERN,		///< kernel messages
			WOLFRAME_SYSLOG_FACILITY_USER,		///< random user-level messages
			WOLFRAME_SYSLOG_FACILITY_MAIL,		///< mail system
			WOLFRAME_SYSLOG_FACILITY_DAEMON,	///< system daemons
			WOLFRAME_SYSLOG_FACILITY_AUTH,		///< security/authorization messages
			WOLFRAME_SYSLOG_FACILITY_SYSLOG,	///< messages generated internally by syslog
			WOLFRAME_SYSLOG_FACILITY_LPR,		///< line printer subsystem
			WOLFRAME_SYSLOG_FACILITY_NEWS,		///< network news subsystem
			WOLFRAME_SYSLOG_FACILITY_UUCP,		///< UUCP subsystem
			WOLFRAME_SYSLOG_FACILITY_CRON,		///< cron/at daemons
			WOLFRAME_SYSLOG_FACILITY_AUTHPRIV,	///< security/authorization messages
			WOLFRAME_SYSLOG_FACILITY_FTP,		///< FTP daemon
			WOLFRAME_SYSLOG_FACILITY_NTP,		///< NTP daemon (on FreeBSD)
			WOLFRAME_SYSLOG_FACILITY_SECURITY,	///< security subsystem (on FreeBSD)
			WOLFRAME_SYSLOG_FACILITY_CONSOLE,	///< /dev/console output (on FreeBSD)
			WOLFRAME_SYSLOG_FACILITY_AUDIT,		///< audit (on Solaris 10)
			WOLFRAME_SYSLOG_FACILITY_LOCAL0,	///< local facility 0
			WOLFRAME_SYSLOG_FACILITY_LOCAL1,	///< local facility 1
			WOLFRAME_SYSLOG_FACILITY_LOCAL2,	///< local facility 2
			WOLFRAME_SYSLOG_FACILITY_LOCAL3,	///< local facility 3
			WOLFRAME_SYSLOG_FACILITY_LOCAL4,	///< local facility 4
			WOLFRAME_SYSLOG_FACILITY_LOCAL5,	///< local facility 5
			WOLFRAME_SYSLOG_FACILITY_LOCAL6,	///< local facility 6
			WOLFRAME_SYSLOG_FACILITY_LOCAL7,	///< local facility 7
			WOLFRAME_SYSLOG_FACILITY_UNDEFINED	///< placeholder
		};

		/// convert string to a syslog facility
		static Facility strToSyslogFacility( const std::string str );
	};

	/// map enum values to strings
	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT > &operator<< ( std::basic_ostream< CharT, TraitsT >& s,
								  SyslogFacility::Facility f )
	{
		static const CharT *const str[] = {
			"KERN", "USER", "MAIL", "DAEMON", "AUTH", "SYSLOG", "LPR",
			"NEWS", "UUCP", "CRON", "AUTHPRIV", "FTP", "NTP", "SECURITY",
			"CONSOLE", "AUDIT", "LOCAL0", "LOCAL1", "LOCAL2", "LOCAL3",
			"LOCAL4", "LOCAL5", "LOCAL6", "LOCAL7" };
		if( static_cast< size_t >( f ) < ( sizeof( str ) / sizeof( *str ) ) ) {
			s << str[f];
		} else {
			s << "Unknown enum used '" << static_cast< int >( f ) << "'";
		}
		return s;
	}

	} // namespace Logging
} // namespace _Wolframe

#endif // _LOG_SYSLOG_FACILITY_HPP_INCLUDED
