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
//
// logger configuration functions
//

#include "standardConfigs.hpp"
#include "config/valueParser.hpp"
#include "config/ConfigurationTree.hpp"
#include "utils/resolvePath.hpp"
#include "logger-v1.hpp"

#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>

using namespace _Wolframe::utils;

namespace _Wolframe {
namespace log {

static const unsigned short DEFAULT_NOF_THREADS = 4;

/// Parse the configuration
bool LoggerConfiguration::parse( const config::ConfigurationTree& pt, const std::string& /*node*/,
				 const module::ModulesDirectory* /*modules*/ )
{
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		// stderr logging
		if ( boost::algorithm::iequals( L1it->first, "stderr" ))	{
			if ( logToStderr )	{
				LOG_ERROR << logPrefix() << "stderr channel already defined";
				return false;
			}
			logToStderr = true;
			stderrLogLevel = log::LogLevel::LOGLEVEL_UNDEFINED;
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "level" ))	{
					std::string s = boost::algorithm::to_upper_copy( L2it->second.get_value<std::string>() );
					boost::algorithm::trim( s );
					log::LogLevel::Level lvl = log::LogLevel::strToLogLevel( s );
					if ( lvl ==  log::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << logPrefix() << "unknown log level: "
							  << L2it->second.get_value<std::string>();
						return false;
					}
					if ( stderrLogLevel != log::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << logPrefix() << "stderr log level already defined. Second value: "
							  << L2it->second.get_value<std::string>();
						return false;
					}
					stderrLogLevel = lvl;
				}
				else
					LOG_WARNING << logPrefix() << "stderr: unknown configuration option: '"
						    << L2it->first << "'";
			}
		}
		// logfile
		else if ( boost::algorithm::iequals( L1it->first, "logFile" ))	{
			if ( logToFile )	{
				LOG_ERROR << logPrefix() << "logfile channel already defined";
				return false;
			}
			logToFile = true;
			logFileLogLevel = log::LogLevel::LOGLEVEL_UNDEFINED;
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
			L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "level" ))	{
					std::string s = boost::algorithm::to_upper_copy( L2it->second.get_value<std::string>() );
					boost::algorithm::trim( s );
					log::LogLevel::Level lvl = log::LogLevel::strToLogLevel( s );
					if ( lvl == log::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << logPrefix() << "logfile: unknown log level: "
							  << L2it->second.get_value<std::string>();
						return false;
					}
					if ( logFileLogLevel != log::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << logPrefix() << "logfile: log level already defined. Second value: "
							  << L2it->second.get_value<std::string>();
						return false;
					}
					logFileLogLevel = lvl;
				}
				else if ( boost::algorithm::iequals( L2it->first, "filename" ))	{
					bool isDefined = ( ! logFile.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, logFile, &isDefined ))
						retVal = false;
				}
				else
					LOG_WARNING << logPrefix() << "logfile: unknown configuration option: '"
						    << L2it->first << "'";
			}
		}
		// syslog
		else if ( boost::algorithm::iequals( L1it->first, "syslog" ))	{
			if ( logToSyslog )	{
				LOG_ERROR << logPrefix() << "syslog channel already defined";
				return false;
			}
			logToSyslog = true;
			syslogLogLevel = log::LogLevel::LOGLEVEL_UNDEFINED;
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
			L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "level" ))	{
					std::string s = boost::algorithm::to_upper_copy( L2it->second.get_value<std::string>() );
					boost::algorithm::trim( s );
					log::LogLevel::Level lvl = log::LogLevel::strToLogLevel( s );
					if ( lvl == log::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << logPrefix() << "syslog: unknown log level: "
							  << L2it->second.get_value<std::string>();
						return false;
					}
					if ( syslogLogLevel != log::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << logPrefix() << "syslog: log level already defined. Second value: "
							  << L2it->second.get_value<std::string>();
						return false;
					}
					syslogLogLevel = lvl;
				}
				else if ( boost::algorithm::iequals( L2it->first, "facility" ))	{
					std::string s = boost::algorithm::to_upper_copy( L2it->second.get_value<std::string>() );
					boost::algorithm::trim( s );
					log::SyslogFacility::Facility fclt = log::SyslogFacility::strToSyslogFacility( s );
					if ( fclt == log::SyslogFacility::WOLFRAME_SYSLOG_FACILITY_UNDEFINED )	{
						LOG_ERROR << logPrefix() << "syslog: unknown facility: "
							  << L2it->second.get_value<std::string>();
						return false;
					}
					if ( syslogFacility != log::SyslogFacility::WOLFRAME_SYSLOG_FACILITY_UNDEFINED )	{
						LOG_ERROR << logPrefix() << "syslog: facility already defined. Second value: "
							  << L2it->second.get_value<std::string>();
						return false;
					}
					syslogFacility = fclt;
				}
				else if ( boost::algorithm::iequals( L2it->first, "ident" ))	{
					bool isDefined = ( ! syslogIdent.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, syslogIdent, &isDefined ))
						retVal = false;
				}
				else
					LOG_WARNING << logPrefix() << "syslog: unknown configuration option: '"
						    << L2it->first << "'";
			}
		}

#if !defined( _WIN32 )
		else if ( boost::algorithm::iequals( L1it->first, "eventlog" ))	{
			LOG_WARNING << logPrefix() << "event log is defined only on Windows";
		}
#else // if !defined( _WIN32 )
		// event log
		else if ( boost::algorithm::iequals( L1it->first, "eventlog" ))	{
			if ( logToEventlog )	{
				LOG_ERROR << logPrefix() << "eventlog channel already defined";
				return false;
			}
			logToEventlog = true;
			eventlogLogLevel = log::LogLevel::LOGLEVEL_UNDEFINED;
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
			L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "level" ))	{
					std::string s = boost::algorithm::to_upper_copy( L2it->second.get_value<std::string>() );
					boost::algorithm::trim( s );
					log::LogLevel::Level lvl = log::LogLevel::strToLogLevel( s );
					if ( lvl == log::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << logPrefix() << "eventlog: unknown log level: "
							  << L2it->second.get_value<std::string>();
						return false;
					}
					if ( eventlogLogLevel != log::LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << logPrefix() << "eventlog: log level already defined. Second value: "
							  << L2it->second.get_value<std::string>();
						return false;
					}
					eventlogLogLevel = lvl;
				}
				else if ( boost::algorithm::iequals( L2it->first, "name" ))	{
					bool isDefined = ( ! eventlogLogName.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, eventlogLogName, &isDefined ))
						retVal = false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "source" ))	{
					bool isDefined = ( ! eventlogSource.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, eventlogSource, &isDefined ))
						retVal = false;
				}
				else
					LOG_WARNING << logPrefix() << "syslog: unknown configuration option: '"
						    << L2it->first << "'";
			}
		}
#endif	// defined( _WIN32 )
		// unknown log method
		else
			LOG_WARNING << logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
	}
	return retVal;
}


LoggerConfiguration::LoggerConfiguration() : ConfigurationBase( "Logging", NULL, "Logging" )
{
	logToStderr = false;
	stderrLogLevel = log::LogLevel::LOGLEVEL_UNDEFINED;
	logToFile = false;
	// std::string		logFile;
	logFileLogLevel = log::LogLevel::LOGLEVEL_UNDEFINED;
	// std::string		logFileIdent;
	logToSyslog = false;
	syslogFacility = log::SyslogFacility::WOLFRAME_SYSLOG_FACILITY_UNDEFINED;
	syslogLogLevel = log::LogLevel::LOGLEVEL_UNDEFINED;
	// std::string		syslogIdent;
#if defined( _WIN32 )
	logToEventlog = false;
	// std::string		eventlogLogName;
	// std::string		eventlogSource;
	eventlogLogLevel = log::LogLevel::LOGLEVEL_UNDEFINED;
#endif // defined( _WIN32 )
}


void LoggerConfiguration::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
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
void LoggerConfiguration::foreground( log::LogLevel::Level debugLevel, bool useConfig )
{
	logToStderr = true;
	if ( debugLevel != log::LogLevel::LOGLEVEL_UNDEFINED )
		stderrLogLevel = debugLevel;

	if ( ! useConfig )	{
		logToFile = false;
		logFile.clear();
		logFileLogLevel = log::LogLevel::LOGLEVEL_UNDEFINED;
		logFileIdent.clear();
		logToSyslog = false;
		syslogFacility = log::SyslogFacility::WOLFRAME_SYSLOG_FACILITY_UNDEFINED;
		syslogLogLevel = log::LogLevel::LOGLEVEL_UNDEFINED;
		syslogIdent.clear();
	}
}


void LoggerConfiguration::setCanonicalPathes( const std::string& refPath )
{
	using namespace boost::filesystem;

	if ( ! logFile.empty() )	{
		std::string oldPath = logFile;
		logFile = utils::getCanonicalPath( logFile, refPath );
		if ( oldPath != logFile )	{
/*MBa ?!?*/		LOG_NOTICE << logPrefix() << "Using absolute filename '" << logFile
				   << "' instead of '" << oldPath << "'";
		}
	}
}

}} // namespace _Wolframe::log
