//
// appConfig.cpp
//

#include "appConfig.hpp"
#include "commandLine.hpp"
#include "configFile.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>


namespace _SMERP {

	bool getBoolValue( boost::property_tree::ptree::const_iterator it, const std::string& module,
			   const std::string& name, bool& value, std::ostream& os )
	{
		std::string s = it->second.get_value<std::string>();
		boost::to_upper( s );
		boost::trim( s );
		if ( s == "NO" || s == "FALSE" || s == "0" || s == "OFF" )	{
			value = false;
			return true;
		}
		if ( s == "YES" || s == "TRUE" || s == "1" || s == "ON" )	{
			value = true;
			return true;
		}
		os << module << ": invalid logical value for " << name << ": <"
				<< it->second.get_value<std::string>() << ">";
		return false;
	}

	bool getStringValue( boost::property_tree::ptree::const_iterator it, const std::string& module,
			     const std::string& name, std::string& value, std::ostream& os )
	{
		if ( !value.empty() )	{
			os << module << ": " << name << " redefined";
			return false;
		}
		value = it->second.get_value<std::string>();
		if ( value.empty() )	{
			os << module << ": invalid " << name << ": <"
					<< it->second.get_value<std::string>() << ">";
			return false;
		}
		return true;
	}

	bool getUnsignedShortValue( boost::property_tree::ptree::const_iterator it, const std::string& module,
				    const std::string& name, unsigned short& value, std::ostream& os )
	{
		if ( value != 0 )	{
			os << module << ": " << name << " redefined";
			return false;
		}
		value = it->second.get_value<unsigned short>();
		if ( value == 0 )	{
			os << module << ": invalid " << name << ": <"
					<< it->second.get_value<std::string>() << ">";
			return false;
		}
		return true;
	}


	ApplicationConfiguration::ApplicationConfiguration( const CmdLineConfig& cmdLine, const CfgFileConfig& cfgFile )
	{
		configFile = cfgFile.file;

#if defined(_WIN32)
		// on Windows the user should either use -f or start in the console
		// (assuming an implicit -f) or the service option should contain
		// --service in the startup parameters of the service impling non-foreground
		if( cmdLine.command == _SMERP::CmdLineConfig::RUN_SERVICE )
			foreground = false;
		else
			foreground = true;
#else
		foreground = cmdLine.foreground;
#endif

#if !defined(_WIN32)
		if ( !cmdLine.user.empty())
			user = cmdLine.user;
		else
			user = cfgFile.user;

		if ( !cmdLine.group.empty())
			group = cmdLine.group;
		else
			group = cfgFile.group;
#endif

		pidFile = cfgFile.pidFile;
		serviceName = cfgFile.serviceName;
		serviceDisplayName = cfgFile.serviceDisplayName;
		serviceDescription = cfgFile.serviceDescription;

		threads = cfgFile.threads;
		maxConnections = cfgFile.maxConnections;

		address = cfgFile.address;
		SSLaddress = cfgFile.SSLaddress;

		dbConfig = cfgFile.dbConfig;
		logConfig = cfgFile.logConfig;

//		if ( !foreground )	{
//			logToStderr = cfgFile.logToStderr;
//			stderrLogLevel = cfgFile.stderrLogLevel;
//			logToFile = cfgFile.logToFile;
//			logFile = cfgFile.logFile;
//			logFileLogLevel = cfgFile.logFileLogLevel;
//#if !defined( _WIN32 )
//			logToSyslog = cfgFile.logToSyslog;
//			syslogFacility = cfgFile.syslogFacility;
//			syslogLogLevel = cfgFile.syslogLogLevel;
//#endif	// !defined( _WIN32 )
//#if defined( _WIN32 )
//// Aba, what for?
////			syslogIdent = cfgFile.syslogIdent;
////			logToEventlog = cfgFile.logToEventlog;
//#endif	//defined( _WIN32 )
//		}
//		else	{
//			logToStderr = true;
//			stderrLogLevel = cmdLine.debugLevel;
//			logToFile = false;
//#if !defined( _WIN32 )
//			logToSyslog = false;
//#endif	// !defined( _WIN32 )
//#if defined( _WIN32 )
//			logToEventlog = false;
//#endif	// defined( _WIN32 )
//		}

//#if defined( _WIN32 )
//		// we need the data always (for running, --install and --remove,
//		// which run in foreground)
//		eventlogLogName = cfgFile.eventlogLogName;
//		eventlogSource = cfgFile.eventlogSource;
//		eventlogLogLevel = cfgFile.eventlogLogLevel;
//#endif	// defined( _WIN32 )
	}


///********************************************************************************************************

// Server configuration functions
	void ServerConfiguration::print( std::ostream& os ) const
	{
		// Unix daemon
#if !defined(_WIN32)
		os << "Run as " << (user.empty() ? "(not specified)" : user) << ":"
				<< (group.empty() ? "(not specified)" : group) << std::endl;
		os << "PID file: " << pidFile << std::endl;
#else
		// Windows service
		os << "When run as service" << std::endl
				<< "  Name: " << serviceName << std::endl
				<< "  Displayed name: " << serviceDisplayName << std::endl
				<< "  Description: " << serviceDescription << std::endl;
#endif
		os << "Number of threads: " << threads << std::endl;
		os << "Maximum number of connections: " << maxConnections << std::endl;

		os << "Network" << std::endl;
		if ( address.size() > 0 )	{
			std::list<Network::ServerTCPendpoint>::const_iterator it = address.begin();
			os << "  Unencrypted: " << it->toString() << std::endl;
			for ( ++it; it != address.end(); ++it )
				os << "               " << it->toString() << std::endl;
		}
		if ( SSLaddress.size() > 0 )	{
			std::list<Network::ServerSSLendpoint>::const_iterator it = SSLaddress.begin();
			os << "          SSL: " << it->toString() << std::endl;
			os << "                  certificate: " << (it->certificate().empty() ? "(none)" : it->certificate()) << std::endl;
			os << "                  key file: " << (it->key().empty() ? "(none)" : it->key()) << std::endl;
			os << "                  CA directory: " << (it->CAdirectory().empty() ? "(none)" : it->CAdirectory()) << std::endl;
			os << "                  CA chain file: " << (it->CAchain().empty() ? "(none)" : it->CAchain()) << std::endl;
			os << "                  verify client: " << (it->verifyClientCert() ? "yes" : "no") << std::endl;
			for ( ++it; it != SSLaddress.end(); ++it )	{
				os << "               " << it->toString() << std::endl;
				os << "                  certificate: " << (it->certificate().empty() ? "(none)" : it->certificate()) << std::endl;
				os << "                  key file: " << (it->key().empty() ? "(none)" : it->key()) << std::endl;
				os << "                  CA directory: " << (it->CAdirectory().empty() ? "(none)" : it->CAdirectory()) << std::endl;
				os << "                  CA chain file: " << (it->CAchain().empty() ? "(none)" : it->CAchain()) << std::endl;
				os << "                  verify client: " << (it->verifyClientCert() ? "yes" : "no") << std::endl;
			}
		}
	}

	/// Check if the server configuration makes sense
	bool ServerConfiguration::check( std::ostream& os ) const
	{
		bool	correct = true;

		for ( std::list<Network::ServerSSLendpoint>::const_iterator it = SSLaddress.begin();
									it != SSLaddress.end(); ++it )	{
			// if it listens to SSL a certificate file and a key file are required
			if ( it->certificate().empty() )	{
				os << "No SSL certificate specified for " << it->toString() << std::endl;
				correct = false;
			}
			if ( it->key().empty() )	{
				os << "No SSL key specified for " << it->toString() << std::endl;
				correct = false;
			}
			// verify client SSL certificate needs either certificate dir or chain file
			if ( it->verifyClientCert() && it->CAdirectory().empty() && it->CAchain().empty() )	{
				os << "Client SSL certificate verification requested but no CA "
						"directory or CA chain file specified for "
						<< it->toString() << std::endl;
				correct = false;
			}
		}
		return correct;
	}


#if !defined(_WIN32)
	/// Override the server configuration with command line arguments
	void ServerConfiguration::override( std::string usr, std::string grp )
	{
		if ( !usr.empty())
			user = usr;
		if ( !grp.empty())
			group = grp;
	}
#endif


//----- Logger configuration functions---------------------------------------------------------------------------------

	LoggerConfiguration::LoggerConfiguration( std::string node, std::string header )
		: ConfigurationBase( node, header )
	{
		logToStderr = false;
		stderrLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
		logToFile = false;
		// std::string		logFile;
		logFileLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
		// std::string		logFileIdent;
#if !defined( _WIN32 )
		logToSyslog = false;
		syslogFacility = SyslogFacility::_SMERP_SYSLOG_FACILITY_UNDEFINED;
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
		os << displayStr() << std::endl;
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
	bool LoggerConfiguration::check( std::ostream& os ) const
	{
		// if log to file is requested then a file must be specified
		if ( logToFile && logFile.empty() )	{
			os << "Log to file requested but no log file specified";
			return false;
		}
		return true;
	}


	bool LoggerConfiguration::parse( boost::property_tree::ptree& pt, std::ostream& os )
	{
		for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
			// stderr logging
			if ( boost::algorithm::iequals( L1it->first, "stderr" ))	{
				if ( logToStderr )	{
					os << displayStr() << ": stderr channel already defined";
					return false;
				}
				logToStderr = true;
				stderrLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
				for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
										L2it != L1it->second.end(); L2it++ )	{
					if ( boost::algorithm::iequals( L2it->first, "level" ))	{
						LogLevel::Level lvl = LogLevel::str2LogLevel( L2it->second.get_value<std::string>() );
						if ( lvl ==  LogLevel::LOGLEVEL_UNDEFINED )	{
							os << displayStr() << ": unknown log level: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						if ( stderrLogLevel != LogLevel::LOGLEVEL_UNDEFINED )	{
							os << displayStr() << ": stderr log level already defined. Second value: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						stderrLogLevel = lvl;
					}
					else	{
						os << displayStr() << ": stderr: unknown configuration option: <"
								<< L2it->first << ">";
						return false;

					}
				}
			}
			// logfile
			else if ( boost::algorithm::iequals( L1it->first, "logFile" ))	{
				if ( logToFile )	{
					os << displayStr() << ": logfile channel already defined";
					return false;
				}
				logToFile = true;
				logFileLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
				for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
										L2it != L1it->second.end(); L2it++ )	{
					if ( boost::algorithm::iequals( L2it->first, "level" ))	{
						LogLevel::Level lvl = LogLevel::str2LogLevel( L2it->second.get_value<std::string>() );
						if ( lvl ==  LogLevel::LOGLEVEL_UNDEFINED )	{
							os << displayStr() << ": logfile: unknown log level: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						if ( logFileLogLevel != LogLevel::LOGLEVEL_UNDEFINED )	{
							os << displayStr() << ": logfile: log level already defined. Second value: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						logFileLogLevel = lvl;
					}
					else if ( boost::algorithm::iequals( L2it->first, "filename" ))	{
						if ( ! logFile.empty() )	{
							os << displayStr() << ": log file already defined. Second value: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						std::string fName = L2it->second.get_value<std::string>();
						if ( fName.empty() )	{
							os << displayStr() << ": logfile: empty filename";
							return false;
						}
						if ( ! boost::filesystem::path( fName ).is_absolute() )	{
							os << displayStr() << ": logfile: filename must be absolute: " << fName;
							return false;
						}
						logFile = fName;
					}
					else	{
						os << displayStr() << ": logfile: unknown configuration option: <"
								<< L2it->first << ">";
						return false;

					}
				}
			}
#if !defined( _WIN32 )
			// syslog
			else if ( boost::algorithm::iequals( L1it->first, "syslog" ))	{
				if ( logToSyslog )	{
					os << displayStr() << ": syslog channel already defined";
					return false;
				}
				logToSyslog = true;
				syslogLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
				for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
										L2it != L1it->second.end(); L2it++ )	{
					if ( boost::algorithm::iequals( L2it->first, "level" ))	{
						LogLevel::Level lvl = LogLevel::str2LogLevel( L2it->second.get_value<std::string>() );
						if ( lvl ==  LogLevel::LOGLEVEL_UNDEFINED )	{
							os << displayStr() << ": unknown log level: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						if ( logFileLogLevel != LogLevel::LOGLEVEL_UNDEFINED )	{
							os << displayStr() << ": logfile log level already defined. Second value: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						logFileLogLevel = lvl;
					}
					else if ( boost::algorithm::iequals( L2it->first, "filename" ))	{
						if ( ! logFile.empty() )	{
							os << displayStr() << ": log file already defined. Second value: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						std::string fName = L2it->second.get_value<std::string>();
						if ( fName.empty() )	{
							os << displayStr() << ": logfile: empty filename";
							return false;
						}
						if ( ! boost::filesystem::path( fName ).is_absolute() )	{
							os << displayStr() << ": logfile: filename must be absolute: " << fName;
							return false;
						}
						logFile = fName;
					}
					else	{
						os << displayStr() << ": logfile: unknown configuration option: <"
								<< L2it->first << ">";
						return false;

					}
				}
			}
#endif	// !defined( _WIN32 )
#if defined( _WIN32 )
			else if ()...
#endif	// defined( _WIN32 )
			// unknown log method
			else	{
				os << displayStr() << ": unknown configuration option: <" << L1it->first << ">";
				return false;
			}
		}
		return true;
	}

//#if !defined( _WIN32 )
//		if ( pt.get_child_optional( "logging.syslog" ))	{
//			logToSyslog = true;
//			std::string str = pt.get<std::string>( "logging.syslog.facility", "LOCAL4" );

//			if ( ( syslogFacility = SyslogFacility::str2SyslogFacility( str )) == SyslogFacility::_SMERP_SYSLOG_FACILITY_UNDEFINED )	{
//				os << "unknown syslog facility \"" << str << "\"";
//				return false;
//			}
//			str = pt.get<std::string>( "logging.syslog.level", "NOTICE" );

//			if ( ( syslogLogLevel = LogLevel::str2LogLevel( str )) == LogLevel::LOGLEVEL_UNDEFINED )	{
//				os << "unknown log level \"" << str << " for syslog";
//				return false;
//			}

//			syslogIdent = pt.get<std::string>( "logging.syslog.ident", "smerpd" );
//		}
//		else
//			logToSyslog = false;
//#endif	// !defined( _WIN32 )

//#if defined( _WIN32 )
//		if ( pt.get_child_optional( "logging.eventlog" )) {
//			logToEventlog = true;
//			eventlogLogName = pt.get<std::string>( "logging.eventlog.name", "smerpd" );
//			eventlogSource = pt.get<std::string>( "logging.eventlog.source", "unknown" );
//			std::string str = pt.get<std::string>( "logging.eventlog.level", "NOTICE" );
//
//			if ( ( eventlogLogLevel = LogLevel::str2LogLevel( str )) == LogLevel::LOGLEVEL_UNDEFINED )	{
//				os << "unknown log level \"" << s << " for Event Log";
//				return false;
//			}
//		}
//		else
//			logToEventlog = false;
//#endif	// defined( _WIN32 )
//	}


//----- Database configuration functions---------------------------------------------------------------------------------

	void DatabaseConfiguration::print( std::ostream& os ) const
	{
		os << displayStr() << std::endl;
		if ( host.empty())
			os << "   DB host: local unix domain socket" << std::endl;
		else
			os << "   DB host: " << host << ":" << port << std::endl;
		os << "   DB name: " << (name.empty() ? "(not specified - server user default)" : name) << std::endl;
		os << "   DB user / password: " << (user.empty() ? "(not specified - same as server user)" : user) << " / "
						<< (password.empty() ? "(not specified - no password used)" : password) << std::endl;
	}

	/// Check if the database configuration makes sense
	bool DatabaseConfiguration::check( std::ostream& ) const
	{
		return true;
	}


	bool DatabaseConfiguration::parse( boost::property_tree::ptree& pt, std::ostream& os )
	{
		for ( boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++ )	{
			if ( boost::algorithm::iequals( it->first, "host" ))	{
				if ( !getStringValue( it, displayStr(), "host", host, os ))		return false;
			}
			else if ( boost::algorithm::iequals( it->first, "port" ))	{
				if ( !getUnsignedShortValue( it, displayStr(), "port", port, os ))	return false;
			}
			else if ( boost::algorithm::iequals( it->first, "name" ))	{
				if ( !getStringValue( it, displayStr(), "name", name, os ))		return false;
			}
			else if ( boost::algorithm::iequals( it->first, "user" ))	{
				if ( !getStringValue( it, displayStr(), "user", user, os ))		return false;
			}
			else if ( boost::algorithm::iequals( it->first, "password" ))	{
				if ( !getStringValue( it, displayStr(), "password", password, os ))	return false;
			}
			else	{
				os << displayStr() << ": unknown configuration option: <" << it->first << ">";
				return false;
			}
		}
		return true;
	}

///********************************************************************************************************


	void ApplicationConfiguration::print( std::ostream& os ) const
	{

		os << "Configuration file: " << configFile << std::endl;
// Unix daemon
#if !defined(_WIN32)
		os << "Run in foreground: " << (foreground ? "yes" : "no") << std::endl;
		os << "Run as " << (user.empty() ? "(not specified)" : user) << ":"
				<< (group.empty() ? "(not specified)" : group) << std::endl;
		os << "PID file: " << pidFile << std::endl;
#else
// Windows service
		os << "When run as service" << std::endl
			<< "  Name: " << serviceName << std::endl
			<< "  Displayed name: " << serviceDisplayName << std::endl
			<< "  Description: " << serviceDescription << std::endl;
#endif
		os << "Number of threads: " << threads << std::endl;
		os << "Maximum number of connections: " << maxConnections << std::endl;

		os << "Network" << std::endl;
		if ( address.size() > 0 )	{
			std::list<Network::ServerTCPendpoint>::const_iterator it = address.begin();
			os << "  Unencrypted: " << it->toString() << std::endl;
			for ( ++it; it != address.end(); ++it )
				os << "               " << it->toString() << std::endl;
		}
		if ( SSLaddress.size() > 0 )	{
			std::list<Network::ServerSSLendpoint>::const_iterator it = SSLaddress.begin();
			os << "          SSL: " << it->toString() << std::endl;
			os << "                  certificate: " << (it->certificate().empty() ? "(none)" : it->certificate()) << std::endl;
			os << "                  key file: " << (it->key().empty() ? "(none)" : it->key()) << std::endl;
			os << "                  CA directory: " << (it->CAdirectory().empty() ? "(none)" : it->CAdirectory()) << std::endl;
			os << "                  CA chain file: " << (it->CAchain().empty() ? "(none)" : it->CAchain()) << std::endl;
			os << "                  verify client: " << (it->verifyClientCert() ? "yes" : "no") << std::endl;
			for ( ++it; it != SSLaddress.end(); ++it )	{
				os << "               " << it->toString() << std::endl;
				os << "                  certificate: " << (it->certificate().empty() ? "(none)" : it->certificate()) << std::endl;
				os << "                  key file: " << (it->key().empty() ? "(none)" : it->key()) << std::endl;
				os << "                  CA directory: " << (it->CAdirectory().empty() ? "(none)" : it->CAdirectory()) << std::endl;
				os << "                  CA chain file: " << (it->CAchain().empty() ? "(none)" : it->CAchain()) << std::endl;
				os << "                  verify client: " << (it->verifyClientCert() ? "yes" : "no") << std::endl;
			}
		}

		dbConfig->print( os );
		logConfig->print( os );
	}

	/// Check if the application configuration makes sense
	bool ApplicationConfiguration::check()
	{
		std::stringstream	errStr;

		// check logging
		if ( ! logConfig->check( errStr ))	{
			errMsg_ = errStr.str();
			return false;
		}

		return true;
	}

} // namespace _SMERP
