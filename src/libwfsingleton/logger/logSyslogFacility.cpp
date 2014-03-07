/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///
/// \file logSyslogFacility.cpp
/// \brief The unix system logger facilities
///

#include "logger/logSyslogFacility.hpp"

namespace _Wolframe {
namespace log {

SyslogFacility::Facility SyslogFacility::strToSyslogFacility( const std::string& s )
{
	if( s == "KERN" )		return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_KERN;
	else if( s == "USER" )		return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_USER;
	else if( s == "MAIL" )		return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_MAIL;
	else if( s == "DAEMON" )	return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_DAEMON;
	else if( s == "AUTH" )		return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_AUTH;
	else if( s == "SYSLOG" )	return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_SYSLOG;
	else if( s == "LPR" )		return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LPR;
	else if( s == "NEWS" )		return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_NEWS;
	else if( s == "UUCP" )		return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_UUCP;
	else if( s == "CRON" )		return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_CRON;
	else if( s == "AUTHPRIV" )	return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_AUTHPRIV;
	else if( s == "FTP" )		return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_FTP;
	else if( s == "NTP" )		return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_NTP;
	else if( s == "SECURITY" )	return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_SECURITY;
	else if( s == "CONSOLE" )	return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_CONSOLE;
	else if( s == "AUDIT" )		return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_AUDIT;
	else if( s == "LOCAL0" )	return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL0;
	else if( s == "LOCAL1" )	return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL1;
	else if( s == "LOCAL2" )	return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL2;
	else if( s == "LOCAL3" )	return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL3;
	else if( s == "LOCAL4" )	return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL4;
	else if( s == "LOCAL5" )	return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL5;
	else if( s == "LOCAL6" )	return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL6;
	else if( s == "LOCAL7" )	return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL7;
	else				return SyslogFacility::WOLFRAME_SYSLOG_FACILITY_UNDEFINED;
}

}} // namespace _Wolframe::log
