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
	namespace	Configuration	{


LoggerConfiguration::LoggerConfiguration()
	: ConfigurationBase( "Logging" )
{
	logToStderr = false;
	stderrLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
	logToFile = false;
	// std::string		logFile;
	logFileLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
	// std::string		logFileIdent;
#if !defined( _WIN32 )
	logToSyslog = false;
	syslogFacility = SyslogFacility::_Wolframe_SYSLOG_FACILITY_UNDEFINED;
	syslogLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
	// std::string		syslogIdent;
#else
	logToEventlog = false;
	// std::string		eventlogLogName;
	// std::string		eventlogSource;
	eventlogLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
#endif // !defined( _WIN32 )
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

#if !defined(_WIN32)
	if ( logToSyslog )
		os << "   Log to syslog: facility " << syslogFacility << ", level " << syslogLogLevel << std::endl;
	else
		os << "   Log to syslog: DISABLED" << std::endl;
#endif	// !defined( _WIN32 )

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
void LoggerConfiguration::foreground( LogLevel::Level debugLevel, bool useConfig )
{
	logToStderr = true;
	if ( debugLevel != LogLevel::LOGLEVEL_UNDEFINED )
		stderrLogLevel = debugLevel;

	if ( ! useConfig )	{
		logToFile = false;
		logFile.clear();
		logFileLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
		logFileIdent.clear();
#if !defined( _WIN32 )
		logToSyslog = false;
		syslogFacility = SyslogFacility::_Wolframe_SYSLOG_FACILITY_UNDEFINED;
		syslogLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
		syslogIdent.clear();
#endif // !defined( _WIN32 )
	}
}


bool LoggerConfiguration::parse( const boost::property_tree::ptree& pt, const std::string& /* node */ )
{
	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		// stderr logging
		if ( boost::algorithm::iequals( L1it->first, "stderr" ))	{
			if ( logToStderr )	{
				LOG_ERROR << displayName() << ": stderr channel already defined";
				return false;
			}
			logToStderr = true;
			stderrLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "level" ))	{
					LogLevel::Level lvl = LogLevel::str2LogLevel( L2it->second.get_value<std::string>() );
					if ( lvl ==  LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << displayName() << ": unknown log level: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					if ( stderrLogLevel != LogLevel::LOGLEVEL_UNDEFINED )	{
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
			logFileLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
			L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "level" ))	{
					LogLevel::Level lvl = LogLevel::str2LogLevel( L2it->second.get_value<std::string>() );
					if ( lvl == LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << displayName() << ": logfile: unknown log level: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					if ( logFileLogLevel != LogLevel::LOGLEVEL_UNDEFINED )	{
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
#if defined( _WIN32 )
		// syslog
		else if ( boost::algorithm::iequals( L1it->first, "syslog" ))	{
			LOG_WARNING << displayName() << ": syslog is not defined on Windows";
		}
#else // if defined( _WIN32 )
		// syslog
		else if ( boost::algorithm::iequals( L1it->first, "syslog" ))	{
			if ( logToSyslog )	{
				LOG_ERROR << displayName() << ": syslog channel already defined";
				return false;
			}
			logToSyslog = true;
			syslogLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
			L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "level" ))	{
					LogLevel::Level lvl = LogLevel::str2LogLevel( L2it->second.get_value<std::string>() );
					if ( lvl == LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << displayName() << ": syslog: unknown log level: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					if ( syslogLogLevel != LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << displayName() << ": syslog: log level already defined. Second value: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					syslogLogLevel = lvl;
				}
				else if ( boost::algorithm::iequals( L2it->first, "facility" ))	{
					SyslogFacility::Facility fclt = SyslogFacility::str2SyslogFacility( L2it->second.get_value<std::string>() );
					if ( fclt == SyslogFacility::_Wolframe_SYSLOG_FACILITY_UNDEFINED )	{
						LOG_ERROR << displayName() << ": syslog: unknown facility: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					if ( syslogFacility != SyslogFacility::_Wolframe_SYSLOG_FACILITY_UNDEFINED )	{
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
#endif	// !defined( _WIN32 )

#if !defined( _WIN32 )
		else if ( boost::algorithm::iequals( L1it->first, "eventlog" ))	{
			LOG_WARNING << displayName() << ": event log is defined only on Windows" << std::endl;
		}
#else // if !defined( _WIN32 )
		// event log
		else if ( boost::algorithm::iequals( L1it->first, "eventlog" ))	{
			if ( logToEventlog )	{
				LOG_ERROR << displayName() << ": eventlog channel already defined";
				return false;
			}
			logToEventlog = true;
			eventlogLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
			L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "level" ))	{
					LogLevel::Level lvl = LogLevel::str2LogLevel( L2it->second.get_value<std::string>() );
					if ( lvl == LogLevel::LOGLEVEL_UNDEFINED )	{
						LOG_ERROR << displayName() << ": eventlog: unknown log level: "
								<< L2it->second.get_value<std::string>();
						return false;
					}
					if ( eventlogLogLevel != LogLevel::LOGLEVEL_UNDEFINED )	{
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

	} // namespace Configuration
} // namespace _Wolframe
