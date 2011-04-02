/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
//
// logger configuration functions
//

#include "standardConfigs.hpp"
#include "configHelpers.hpp"
#include "miscUtils.hpp"
#include "logger.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>


namespace _Wolframe	{
namespace Logging	{


LoggerConfiguration::LoggerConfiguration()
	: ConfigurationBase( "Logging" )
{
	logToStderr = false;
	stderrLogLevel = Logging::LogLevel::LOGLEVEL_UNDEFINED;
	logToFile = false;
	// std::string		logFile;
	logFileLogLevel = Logging::LogLevel::LOGLEVEL_UNDEFINED;
	// std::string		logFileIdent;
	logToSyslog = false;
	syslogFacility = Logging::SyslogFacility::WOLFRAME_SYSLOG_FACILITY_UNDEFINED;
	syslogLogLevel = Logging::LogLevel::LOGLEVEL_UNDEFINED;
	// std::string		syslogIdent;
#if defined( _WIN32 )
	logToEventlog = false;
	// std::string		eventlogLogName;
	// std::string		eventlogSource;
	eventlogLogLevel = Logging::LogLevel::LOGLEVEL_UNDEFINED;
#endif // defined( _WIN32 )
}


void LoggerConfiguration::print( std::ostream& os ) const
{
	os << displayName() << std::endl;
	if ( logToStderr )
		os << "   Log to stderr, level " << stderrLogLevel << std::endl;
	else
		os << "   Log to stderr: DISABLED" << std::endl;
	if ( logToFile )
		os << "   Log to file: " << logFile << ", level " << logFileLogLevel << std::endl;
	else
		os << "   Log to file: DISABLED" << std::endl;

	if ( logToSyslog )
		os << "   Log to syslog: facility " << syslogFacility << ", level " << syslogLogLevel << std::endl;
	else
		os << "   Log to syslog: DISABLED" << std::endl;

#if defined(_WIN32)
	if ( logToEventlog )
		os << "   Log to eventlog: name " << eventlogLogName << ", source " << eventlogSource << ", level " << eventlogLogLevel;
	else
		os << "   Log to eventlog: DISABLED" << std::endl;
#endif	// defined( _WIN32 )
}


/// Check if the logger configuration makes sense
bool LoggerConfiguration::check() const
{
	// if log to file is requested then a file must be specified
	if ( logToFile && logFile.empty() )	{
		LOG_ERROR << "Log to file requested but no log file specified";
		return false;
	}
	return true;
}


/// Modify the logging parameters according to the command line
void LoggerConfiguration::foreground( Logging::LogLevel::Level debugLevel, bool useConfig )
{
	logToStderr = true;
	if ( debugLevel != Logging::LogLevel::LOGLEVEL_UNDEFINED )
		stderrLogLevel = debugLevel;

	if ( ! useConfig )	{
		logToFile = false;
		logFile.clear();
		logFileLogLevel = Logging::LogLevel::LOGLEVEL_UNDEFINED;
		logFileIdent.clear();
		logToSyslog = false;
		syslogFacility = Logging::SyslogFacility::WOLFRAME_SYSLOG_FACILITY_UNDEFINED;
		syslogLogLevel = Logging::LogLevel::LOGLEVEL_UNDEFINED;
		syslogIdent.clear();
	}
}


bool LoggerConfiguration::parse( const boost::property_tree::ptree::const_iterator it,
				 const std::string& /* node */ )
{
	for ( boost::property_tree::ptree::const_iterator L1it = it->second.begin();
								L1it != it->second.end(); L1it++ )	{
		// stderr logging
		if ( boost::algorithm::iequals( L1it->first, "stderr" ))	{
			if ( logToStderr )	{
				LOG_ERROR << displayName() << ": stderr channel already defined";
				return false;
			}
			logToStderr = true;
			stderrLogLevel = Logging::LogLevel::LOGLEVEL_UNDEFINED;
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "level" ))	{
					std::string s = boost::algorithm::to_upper_copy( L2it->second.get_value<std::string>() );
					boost::algorithm::trim( s );
					Logging::LogLevel::Level lvl = Logging::LogLevel::strToLogLevel( s );
					if ( lvl ==  Logging::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << displayName() << ": unknown log level: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					if ( stderrLogLevel != Logging::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << displayName() << ": stderr log level already defined. Second value: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					stderrLogLevel = lvl;
				}
				else	{
					LOG_WARNING << displayName() << ": stderr: unknown configuration option: <"
							<< L2it->first << ">";
//					return false;
				}
			}
		}
		// logfile
		else if ( boost::algorithm::iequals( L1it->first, "logFile" ))	{
			if ( logToFile )	{
				LOG_ERROR << displayName() << ": logfile channel already defined";
				return false;
			}
			logToFile = true;
			logFileLogLevel = Logging::LogLevel::LOGLEVEL_UNDEFINED;
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
			L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "level" ))	{
					std::string s = boost::algorithm::to_upper_copy( L2it->second.get_value<std::string>() );
					boost::algorithm::trim( s );
					Logging::LogLevel::Level lvl = Logging::LogLevel::strToLogLevel( s );
					if ( lvl == Logging::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << displayName() << ": logfile: unknown log level: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					if ( logFileLogLevel != Logging::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << displayName() << ": logfile: log level already defined. Second value: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					logFileLogLevel = lvl;
				}
				else if ( boost::algorithm::iequals( L2it->first, "filename" ))	{
					if ( ! logFile.empty() )	{
						LOG_ERROR << displayName() << ": log file already defined. Second value: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					std::string fName = L2it->second.get_value<std::string>();
					if ( fName.empty() )	{
						LOG_ERROR << displayName() << ": logfile: empty filename";
						return false;
					}
					logFile = fName;
					if ( ! boost::filesystem::path( logFile ).is_absolute() )
						LOG_WARNING << displayName() << ": log file is not absolute: " << logFile;
				}
				else	{
					LOG_WARNING << displayName() << ": logfile: unknown configuration option: <"
							<< L2it->first << ">";
//					return false;
				}
			}
		}
		// syslog
		else if ( boost::algorithm::iequals( L1it->first, "syslog" ))	{
			if ( logToSyslog )	{
				LOG_ERROR << displayName() << ": syslog channel already defined";
				return false;
			}
			logToSyslog = true;
			syslogLogLevel = Logging::LogLevel::LOGLEVEL_UNDEFINED;
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
			L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "level" ))	{
					std::string s = boost::algorithm::to_upper_copy( L2it->second.get_value<std::string>() );
					boost::algorithm::trim( s );
					Logging::LogLevel::Level lvl = Logging::LogLevel::strToLogLevel( s );
					if ( lvl == Logging::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << displayName() << ": syslog: unknown log level: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					if ( syslogLogLevel != Logging::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << displayName() << ": syslog: log level already defined. Second value: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					syslogLogLevel = lvl;
				}
				else if ( boost::algorithm::iequals( L2it->first, "facility" ))	{
					std::string s = boost::algorithm::to_upper_copy( L2it->second.get_value<std::string>() );
					boost::algorithm::trim( s );
					Logging::SyslogFacility::Facility fclt = Logging::SyslogFacility::strToSyslogFacility( s );
					if ( fclt == Logging::SyslogFacility::WOLFRAME_SYSLOG_FACILITY_UNDEFINED )	{
						LOG_ERROR << displayName() << ": syslog: unknown facility: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					if ( syslogFacility != Logging::SyslogFacility::WOLFRAME_SYSLOG_FACILITY_UNDEFINED )	{
						LOG_ERROR << displayName() << ": syslog: facility already defined. Second value: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					syslogFacility = fclt;
				}
				else if ( boost::algorithm::iequals( L2it->first, "ident" ))	{
					if ( ! syslogIdent.empty() )	{
						LOG_ERROR << displayName() << ": syslog: ident already defined. Second value: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					std::string ident = L2it->second.get_value<std::string>();
					if ( ident.empty() )	{
						LOG_ERROR << displayName() << ": syslog: empty ident";
						return false;
					}
					syslogIdent = ident;
				}
				else	{
					LOG_WARNING << displayName() << ": syslog: unknown configuration option: <"
							<< L2it->first << ">";
//					return false;
				}
			}
		}

#if !defined( _WIN32 )
		else if ( boost::algorithm::iequals( L1it->first, "eventlog" ))	{
			LOG_WARNING << displayName() << ": event log is defined only on Windows";
		}
#else // if !defined( _WIN32 )
		// event log
		else if ( boost::algorithm::iequals( L1it->first, "eventlog" ))	{
			if ( logToEventlog )	{
				LOG_ERROR << displayName() << ": eventlog channel already defined";
				return false;
			}
			logToEventlog = true;
			eventlogLogLevel = Logging::LogLevel::LOGLEVEL_UNDEFINED;
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
			L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "level" ))	{
					std::string s = boost::algorithm::to_upper_copy( L2it->second.get_value<std::string>() );
					boost::algorithm::trim( s );				
					Logging::LogLevel::Level lvl = Logging::LogLevel::strToLogLevel( s );
					if ( lvl == Logging::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << displayName() << ": eventlog: unknown log level: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					if ( eventlogLogLevel != Logging::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << displayName() << ": eventlog: log level already defined. Second value: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					eventlogLogLevel = lvl;
				}
				else if ( boost::algorithm::iequals( L2it->first, "name" ))	{
					if ( ! eventlogLogName.empty() )	{
						LOG_ERROR << displayName() << ": eventlog: name already defined. Second value: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					std::string eName = L2it->second.get_value<std::string>();
					if ( eName.empty() )	{
						LOG_ERROR << displayName() << ": eventlog: empty name";
						return false;
					}
					eventlogLogName = eName;
				}
				else if ( boost::algorithm::iequals( L2it->first, "source" ))	{
					if ( ! eventlogSource.empty() )	{
						LOG_ERROR << displayName() << ": eventlog: source already defined. Second value: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					std::string eSource = L2it->second.get_value<std::string>();
					if ( eSource.empty() )	{
						LOG_ERROR << displayName() << ": eventlog: empty source";
						return false;
					}
					eventlogSource = eSource;
				}
				else	{
					LOG_WARNING << displayName() << ": syslog: unknown configuration option: <"
							<< L2it->first << ">";
//					return false;
				}
			}
		}
#endif	// defined( _WIN32 )
		// unknown log method
		else	{
			LOG_WARNING << displayName() << ": unknown configuration option: <" << L1it->first << ">";
//			return false;
		}
	}
	return true;
}


void LoggerConfiguration::setCanonicalPathes( const std::string& refPath )
{
	if ( ! logFile.empty() )	{
		if ( ! boost::filesystem::path( logFile ).is_absolute() )
			logFile = resolvePath( boost::filesystem::absolute( logFile,
								boost::filesystem::path( refPath ).branch_path()).string());
		else
			logFile = resolvePath( logFile );
	}
}

}} // namespace _Wolframe::Logging
