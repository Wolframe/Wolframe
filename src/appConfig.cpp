//
// appConfig.cpp
//

#include "appConfig.hpp"
#include "commandLine.hpp"
#include "configFile.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>

namespace _SMERP {

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

		idleTimeout = cfgFile.idleTimeout;
		requestTimeout = cfgFile.requestTimeout;
		answerTimeout = cfgFile.answerTimeout;
		processTimeout = cfgFile.processTimeout;

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
	///
	/// Be aware that this function does NOT test if the configuration
	/// can be used. It only tests if it MAY be valid.
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

	bool ServerConfiguration::test( std::ostream& os ) const
	{
		os << displayStr() << "Not implemented yet !" << std::endl;
		return false;
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
	///
	/// Be aware that this function does NOT test if the configuration
	/// can be used. It only tests if it MAY be valid.
	bool LoggerConfiguration::check( std::ostream& os ) const
	{
		// if log to file is requested then a file must be specified
		if ( logToFile && logFile.empty() )	{
			os << "Log to file requested but no log file specified";
			return false;
		}
		return true;
	}

	bool LoggerConfiguration::test( std::ostream& os ) const
	{
		os << displayStr() << "Not implemented yet !" << std::endl;
		return false;
	}


	bool LoggerConfiguration::parse( boost::property_tree::ptree& pt, std::ostream& os )
	{
		if ( pt.get_child_optional( "logging.stderr" ))	{
			logToStderr = true;
			std::string str = pt.get<std::string>( "logging.stderr.level", "NOTICE" );

			if ( ( stderrLogLevel = LogLevel::str2LogLevel( str )) == LogLevel::LOGLEVEL_UNDEFINED )	{
				os << "unknown log level \"" << str << " for stderr";
				return false;
			}
		}
		else
			logToStderr = false;

		if ( pt.get_child_optional( "logging.logFile" ))	{
			logToFile = true;
			logFile = boost::filesystem::absolute(
					pt.get<std::string>( "logging.logFile.filename", std::string() ),
					boost::filesystem::path( file ).branch_path() ).string();
			std::string str = pt.get<std::string>( "logging.logFile.level", "ERROR" );

			if ( ( logFileLogLevel = LogLevel::str2LogLevel( str )) == LogLevel::LOGLEVEL_UNDEFINED )	{
				os << "unknown log level \"" << str << " for logfile";
				return false;
			}
		}
		else
			logToFile = false;
#if !defined( _WIN32 )
		if ( pt.get_child_optional( "logging.syslog" ))	{
			logToSyslog = true;
			std::string str = pt.get<std::string>( "logging.syslog.facility", "LOCAL4" );

			if ( ( syslogFacility = SyslogFacility::str2SyslogFacility( str )) == SyslogFacility::_SMERP_SYSLOG_FACILITY_UNDEFINED )	{
				os << "unknown syslog facility \"" << str << "\"";
				return false;
			}
			str = pt.get<std::string>( "logging.syslog.level", "NOTICE" );

			if ( ( syslogLogLevel = LogLevel::str2LogLevel( str )) == LogLevel::LOGLEVEL_UNDEFINED )	{
				os << "unknown log level \"" << str << " for syslog";
				return false;
			}

			syslogIdent = pt.get<std::string>( "logging.syslog.ident", "smerpd" );
		}
		else
			logToSyslog = false;
#endif	// !defined( _WIN32 )

#if defined( _WIN32 )
		if ( pt.get_child_optional( "logging.eventlog" )) {
			logToEventlog = true;
			eventlogLogName = pt.get<std::string>( "logging.eventlog.name", "smerpd" );
			eventlogSource = pt.get<std::string>( "logging.eventlog.source", "unknown" );
			std::string str = pt.get<std::string>( "logging.eventlog.level", "NOTICE" );
			std::string s = str;
			boost::trim( s );
			boost::to_upper( s );

			if ( ( eventlogLogLevel = LogLevel::str2LogLevel( s )) == LogLevel::LOGLEVEL_UNDEFINED )	{
				os << "unknown log level \"" << s << " for Event Log";
				return false;
			}
		}
		else
			logToEventlog = false;
#endif	// defined( _WIN32 )
	}


//----- Database configuration functions---------------------------------------------------------------------------------

	void DatabaseConfiguration::print( std::ostream& os ) const
	{
		os << displayStr() << std::endl;
		if ( host.empty())
			os << "   DB host: local unix domain socket" << std::endl;
		else
			os << "   DB host: " << host << ":" << dbPort << std::endl;
		os << "   DB name: " << (name.empty() ? "(not specified - server user default)" : name) << std::endl;
		os << "   DB user / password: " << (user.empty() ? "(not specified - same as server user)" : user) << " / "
						<< (password.empty() ? "(not specified - no password used)" : password) << std::endl;
	}

	/// Check if the database configuration makes sense
	bool DatabaseConfiguration::check( std::ostream& ) const
	{
		return true;
	}

	bool DatabaseConfiguration::test( std::ostream& os ) const
	{
		os << displayStr() << "Not implemented yet !" << std::endl;
		return false;
	}

	bool getString( boost::property_tree::ptree::const_iterator it,
			       const char *name, std::string& value, std::ostream& os )
	{
		if ( !value.empty() )	{
			os << displayStr() << ": " << name << " redefined";
			return false;
		}
		value = it->second.get_value<std::string>();
		if ( value.empty() )	{
			os << displayStr() << ": invalid " << name << ": <"
					<< it->second.get_value<std::string>() << ">";
			return false;
		}
		return true;
	}

	bool getUnsignedShort( boost::property_tree::ptree::const_iterator it,
			       const char *name, unsigned short& value, std::ostream& os )
	{
		if ( value != 0 )	{
			os << displayStr() << ": " << name << " redefined";
			return false;
		}
		value = it->second.get_value<unsigned short>();
		if ( value == 0 )	{
			os << displayStr() << ": invalid " << name << ": <"
					<< it->second.get_value<std::string>() << ">";
			return false;
		}
		return true;
	}

	bool DatabaseConfiguration::parse( boost::property_tree::ptree& pt, std::ostream& os )
	{
		for ( boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++ )	{
			std::string entry = it->first;
			if ( boost::algorithm::iequals( entry, "host" ))	{
				if ( !getString( it, "host", host, os ))		return false;
			}
			else if ( boost::algorithm::iequals( entry, "port" ))	{
				if ( !getUnsignedShort( it, "port", port, os ))		return false;
			}
			else if ( boost::algorithm::iequals( entry, "name" ))	{
				if ( !getString( it, "name", name, os ))		return false;
			}
			else if ( boost::algorithm::iequals( entry, "user" ))	{
				if ( !getString( it, "user", user, os ))		return false;
			}
			else if ( boost::algorithm::iequals( entry, "password" ))	{
				if ( !getString( it, "password", password, os ))	return false;
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

		os << "Timeouts" << std::endl;
		os << "   idle: " << idleTimeout << std::endl;
		os << "   request: " << requestTimeout << std::endl;
		os << "   answer: " << answerTimeout << std::endl;
		os << "   process: " << processTimeout << std::endl;

		dbConfig->print( os );

		os << "Logging" << std::endl;
		if ( logToStderr )
			os << "   Log to stderr, level " << stderrLogLevel << std::endl;
		else
			os << "   Log to stderr: DISABLED" << std::endl;
		if ( logToFile )	{
			os << "   Log to file: " << logFile << ", level " << logFileLogLevel << std::endl;
		}
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

	/// Check if the application configuration makes sense
	///
	/// Be aware that this function does NOT test if the configuration
	/// can be used. It only tests if it MAY be valid.
	bool ApplicationConfiguration::check()
	{
		// if log to file is requested a file must be specified
		if ( logToFile )
			if ( logFile.empty())	{
				errMsg_ = "Log to file requested but no log file specified";
				return false;
			}
		return true;
	}

} // namespace _SMERP
