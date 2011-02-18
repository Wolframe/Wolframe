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

		srvConfig = cfgFile.srvConfig;
		dbConfig = cfgFile.dbConfig;
		logConfig = cfgFile.logConfig;

		if ( foreground )
			logConfig->foreground( cmdLine.debugLevel, cmdLine.useLogConfig );
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


//////////////////////////////////
	bool parse( boost::property_tree::ptree& pt, std::ostream& os )
	{
		return true;
	}
//________________________________
//	BOOST_FOREACH( boost::property_tree::ptree::value_type &v, pt.get_child( "server.listen" ))	{
//		std::string hostStr = v.second.get<std::string>( "address", std::string() );
//		if ( hostStr.empty() )	{
//			errMsg_ = "Interface must be defined";
//			return false;
//		}
//		if ( hostStr == "*" )
//			hostStr = "0.0.0.0";
//		std::string portStr = v.second.get<std::string>( "port", std::string() );
//		if ( portStr.empty() )	{
//			if ( v.first == "socket" )
//				port = DEFAULT_PORT;
//			else
//				port = SSL_DEFAULT_PORT;
//		}
//		else	{
//			try	{
//				port = boost::lexical_cast<unsigned short>( portStr );
//			}
//			catch( boost::bad_lexical_cast& )	{
//				errMsg_ = "Invalid value for port: ";
//				errMsg_ += portStr;
//				return false;
//			}
//			if ( port == 0 )	{
//				errMsg_ = "Port out of range: ";
//				errMsg_ += portStr;
//				return false;
//			}
//		}

//		unsigned maxConn = v.second.get<unsigned>( "maxConnections", 0 );

//		if ( v.first == "socket" )	{
//			Network::ServerTCPendpoint lep( hostStr, port, maxConn );
//			address.push_back( lep );
//		}
//		else if ( v.first == "SSLsocket" )	{
//// get SSL certificate / CA param
//			std::string certFile = boost::filesystem::absolute(
//								v.second.get<std::string>( "certificate", std::string() ),
//								boost::filesystem::path( file ).branch_path() ).string();
//			std::string keyFile = boost::filesystem::absolute(
//								v.second.get<std::string>( "key", std::string() ),
//								boost::filesystem::path( file ).branch_path() ).string();
//			std::string CAdirectory = boost::filesystem::absolute(
//								v.second.get<std::string>( "CAdirectory", std::string() ),
//								boost::filesystem::path( file ).branch_path() ).string();
//			std::string CAchainFile = boost::filesystem::absolute(
//								v.second.get<std::string>( "CAchainFile", std::string() ),
//								boost::filesystem::path( file ).branch_path() ).string();

//			std::string tmpStr;
//			boost::logic::tribool flag = getBoolValue( v.second, "verify", tmpStr );
//			bool verify;
//			if ( flag )
//				verify = true;
//			else if ( !flag )
//				verify = false;
//			else	{
//				verify = true;
//				errMsg_ = "Unknown value \"";
//				errMsg_ += tmpStr;
//				errMsg_ += "\" for SSL verify client. WARNING: enabling verification";
//			}
//			Network::ServerSSLendpoint lep( hostStr, port, maxConn,
//							certFile, keyFile,
//							verify, CAdirectory, CAchainFile );
//			SSLaddress.push_back( lep );
//		}
//		else	{
//			errMsg_ = "Invalid listen type: ";
//			errMsg_ += v.first;
//			return false;
//		}
//	}

//	threads = pt.get<unsigned short>( "server.threads", 4 );
//	maxConnections = pt.get<unsigned short>( "server.maxConnections", 256 );

//	user = pt.get<std::string>( "server.daemon.user", std::string() );
//	group = pt.get<std::string>( "server.daemon.group", std::string() );
//	pidFile = pt.get<std::string>( "server.daemon.pidFile", std::string( ) );

//	serviceName = pt.get<std::string>( "server.service.name", DEFAULT_SERVICE_NAME );
//	serviceDisplayName = pt.get<std::string>( "server.service.displayName", DEFAULT_SERVICE_DISPLAY_NAME );
//	serviceDescription = pt.get<std::string>( "server.service.description", DEFAULT_SERVICE_DESCRIPTION );
// ********************************

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
						if ( lvl == LogLevel::LOGLEVEL_UNDEFINED )	{
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
						if ( lvl == LogLevel::LOGLEVEL_UNDEFINED )	{
							os << displayStr() << ": syslog: unknown log level: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						if ( syslogLogLevel != LogLevel::LOGLEVEL_UNDEFINED )	{
							os << displayStr() << ": syslog: log level already defined. Second value: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						syslogLogLevel = lvl;
					}
					else if ( boost::algorithm::iequals( L2it->first, "facility" ))	{
						SyslogFacility::Facility fclt = SyslogFacility::str2SyslogFacility( L2it->second.get_value<std::string>() );
						if ( fclt == SyslogFacility::_SMERP_SYSLOG_FACILITY_UNDEFINED )	{
							os << displayStr() << ": syslog: unknown facility: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						if ( syslogFacility != SyslogFacility::_SMERP_SYSLOG_FACILITY_UNDEFINED )	{
							os << displayStr() << ": syslog: facility already defined. Second value: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						syslogFacility = fclt;
					}
					else if ( boost::algorithm::iequals( L2it->first, "ident" ))	{
						if ( ! syslogIdent.empty() )	{
							os << displayStr() << ": syslog: ident already defined. Second value: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						std::string ident = L2it->second.get_value<std::string>();
						if ( ident.empty() )	{
							os << displayStr() << ": syslog: empty ident";
							return false;
						}
						syslogIdent = ident;
					}
					else	{
						os << displayStr() << ": syslog: unknown configuration option: <"
								<< L2it->first << ">";
						return false;
					}
				}
			}
#endif	// !defined( _WIN32 )
#if defined( _WIN32 )
			// event log
			else if ( boost::algorithm::iequals( L1it->first, "eventlog" ))	{
				if ( logToEventlog )	{
					os << displayStr() << ": eventlog channel already defined";
					return false;
				}
				logToEventlog = true;
				eventlogLogLevel = LogLevel::LOGLEVEL_UNDEFINED;
				for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
										L2it != L1it->second.end(); L2it++ )	{
					if ( boost::algorithm::iequals( L2it->first, "level" ))	{
						LogLevel::Level lvl = LogLevel::str2LogLevel( L2it->second.get_value<std::string>() );
						if ( lvl == LogLevel::LOGLEVEL_UNDEFINED )	{
							os << displayStr() << ": eventlog: unknown log level: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						if ( eventlogLogLevel != LogLevel::LOGLEVEL_UNDEFINED )	{
							os << displayStr() << ": eventlog: log level already defined. Second value: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						eventlogLogLevel = lvl;
					}
					else if ( boost::algorithm::iequals( L2it->first, "name" ))	{
						if ( ! eventlogLogName.empty() )	{
							os << displayStr() << ": eventlog: name already defined. Second value: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						std::string eName = L2it->second.get_value<std::string>();
						if ( eName.empty() )	{
							os << displayStr() << ": eventlog: empty name";
							return false;
						}
						eventlogLogName = eName;
					}
					else if ( boost::algorithm::iequals( L2it->first, "source" ))	{
						if ( ! eventlogSource.empty() )	{
							os << displayStr() << ": eventlog: source already defined. Second value: "
								<< L2it->second.get_value<std::string>();
							return false;
						}
						std::string eSource = L2it->second.get_value<std::string>();
						if ( eSource.empty() )	{
							os << displayStr() << ": eventlog: empty source";
							return false;
						}
						eventlogSource = eSource;
					}
					else	{
						os << displayStr() << ": syslog: unknown configuration option: <"
								<< L2it->first << ">";
						return false;
					}
				}
			}
#endif	// defined( _WIN32 )
			// unknown log method
			else	{
				os << displayStr() << ": unknown configuration option: <" << L1it->first << ">";
				return false;
			}
		}
		return true;
	}


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
