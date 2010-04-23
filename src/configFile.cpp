//
// configFile.cpp
//

#include "configFile.hpp"
#include "miscStruct.hpp"

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>

#include <vector>
#include <string>


static const unsigned short	DEFAULT_PORT = 7660;
static const unsigned short	SSL_DEFAULT_PORT = 7660;

static const char*		DEFAULT_SERVICE_NAME = "smerp";
static const char*		DEFAULT_SERVICE_DISPLAY_NAME = "Smerp Daemon";
static const char*		DEFAULT_SERVICE_DESCRIPTION = "a daemon for smerping";

static boost::filesystem::path resolvePath(const boost::filesystem::path& p)
{
	boost::filesystem::path result;
	for ( boost::filesystem::path::iterator it = p.begin(); it != p.end(); ++it )	{
		if ( *it == ".." )	{
			// /a/b/.. is not necessarily /a if b is a symbolic link
			if ( boost::filesystem::is_symlink( result ) )
				result /= *it;
			// /a/b/../.. is not /a/b/.. under most circumstances
			// We can end up with ..s in our result because of symbolic links
			else if( result.filename() == ".." )
				result /= *it;
			// Otherwise it should be safe to resolve the parent
			else
				result = result.parent_path();
		}
		else if( *it == "." )	{
			// Ignore
		}
		else {
			// Just cat other path entries
			result /= *it;
		}
	}
	return result;
}


namespace _SMERP {

	const char* CfgFileConfig::chooseFile( const char *globalFile, const char *userFile, const char *localFile )
	{
		if ( globalFile != NULL )
			if ( boost::filesystem::exists( globalFile ))
				return globalFile;
		if ( userFile != NULL )
			if ( boost::filesystem::exists( userFile ))
				return userFile;
		if ( localFile != NULL )
			if ( boost::filesystem::exists( localFile ))
				return localFile;
		return NULL;
	}

	CfgFileConfig::CfgFileConfig()
	{
	}


	bool CfgFileConfig::parse ( const char *filename )
	{
		std::string	tmpStr, portStr;
		unsigned short	port;

		file = resolvePath( boost::filesystem::complete( filename )).string();
		if ( !boost::filesystem::exists( file ))	{
			errMsg_ = "Configuration file ";
			errMsg_ += file;
			errMsg_ += " does not exist.";
			return false;
		}

		// Create an empty property tree object
		using boost::property_tree::ptree;
		ptree	pt, rootTree;

		// Load the XML file into the property tree. If reading fails
		// (cannot open file, parse error), an exception is thrown.
		read_xml( filename, rootTree );
		pt = rootTree.get_child( "configuration" );

		//
		BOOST_FOREACH( ptree::value_type &v, pt.get_child( "server.listen" ))	{
			tmpStr = v.second.get<std::string>( "address", std::string() );
			if ( tmpStr.empty() )	{
				errMsg_ = "Interface must be defined";
				return false;
			}
			if ( tmpStr == "*" )
				tmpStr = "0.0.0.0";
			portStr = v.second.get<std::string>( "port", std::string() );
			if ( portStr.empty() )	{
				if ( v.first == "socket" )
					port = DEFAULT_PORT;
				else
					port = SSL_DEFAULT_PORT;
			}
			else	{
				try	{
					port = boost::lexical_cast<unsigned short>( portStr );
				}
				catch( boost::bad_lexical_cast& )	{
					errMsg_ = "Invalid value for port: ";
					errMsg_ += portStr;
					return false;
				}
				if ( port == 0 )	{
					errMsg_ = "Port out of range: ";
					errMsg_ += portStr;
					return false;
				}
			}

			if ( v.first == "socket" )	{
				struct localEndpoint lep( tmpStr, port );
				address.push_back( lep );
			}
			else if ( v.first == "SSLsocket" )	{
				struct localSSLendpoint lep( tmpStr, port );
// get SSL certificate / CA param
				lep.certFile = boost::filesystem::complete(
									v.second.get<std::string>( "certificate", std::string() ),
									boost::filesystem::path( file ).branch_path() ).string();
				lep.keyFile = boost::filesystem::complete(
									v.second.get<std::string>( "key", std::string() ),
									boost::filesystem::path( file ).branch_path() ).string();
				lep.CAdirectory = boost::filesystem::complete(
									v.second.get<std::string>( "CAdirectory", std::string() ),
									boost::filesystem::path( file ).branch_path() ).string();
				lep.CAchainFile = boost::filesystem::complete(
									v.second.get<std::string>( "CAchainFile", std::string() ),
									boost::filesystem::path( file ).branch_path() ).string();

				tmpStr = v.second.get<std::string>( "verify", std::string() );
				boost::to_upper( tmpStr );
				boost::trim( tmpStr );
				if ( tmpStr == "NO" || tmpStr == "FALSE" || tmpStr == "0" || tmpStr == "OFF" )
					lep.verify = false;
				else	{
					lep.verify = true;
					if ( tmpStr != "YES" && tmpStr != "TRUE" && tmpStr != "1" && tmpStr == "ON" )	{
						errMsg_ = "Unknown value \"";
						errMsg_ += tmpStr;
						errMsg_ += "\" for SSL verify client. WARNING: enabling verification";
					}
				}

				SSLaddress.push_back( lep );
			}
			else	{
				errMsg_ = "Invalid listen type: ";
				errMsg_ += v.first;
				return false;
			}
		}

		threads = pt.get<unsigned short>( "server.threads", 4 );
		maxClients = pt.get<unsigned short>( "server.maxClients", 256 );

		user = pt.get<std::string>( "server.daemon.user", std::string() );
		group = pt.get<std::string>( "server.daemon.group", std::string() );
		pidFile = pt.get<std::string>( "server.daemon.pidFile", std::string( ) );

		serviceName = pt.get<std::string>( "server.service.name", DEFAULT_SERVICE_NAME );
		serviceDisplayName = pt.get<std::string>( "server.service.displayName", DEFAULT_SERVICE_DISPLAY_NAME );
		serviceDescription = pt.get<std::string>( "server.service.description", DEFAULT_SERVICE_DESCRIPTION );

		idleTimeout = pt.get<unsigned>( "server.timeout.idle", 900 );
		requestTimeout = pt.get<unsigned>( "server.timeout.request", 30 );
		answerTimeout = pt.get<unsigned>( "server.timeout.answer", 30 );
		processTimeout = pt.get<unsigned>( "server.timeout.process", 30 );

// database
		dbHost = pt.get<std::string>( "database.host", std::string() );
		dbPort = pt.get<unsigned short>( "database.port", 0 );
		dbName = pt.get<std::string>( "database.name", std::string() );
		dbUser = pt.get<std::string>( "database.user", std::string() );
		dbPassword = pt.get<std::string>( "database.password", std::string() );

		if ( pt.get_child_optional( "logging.stderr" ))	{
			logToStderr = true;
			std::string s = pt.get<std::string>( "logging.stderr.level", "NOTICE" );
			if ( ( stderrLogLevel = LogLevel::str2LogLevel( s )) == LogLevel::_SMERP_UNDEFINED )	{
				errMsg_ = "unknown log level \"";
				errMsg_ += s;
				errMsg_ += " for stderr";
				return false;
			}
		}
		else
			logToStderr = false;

		if ( pt.get_child_optional( "logging.logFile" ))	{
			logToFile = true;
			logFile = boost::filesystem::complete(
						pt.get<std::string>( "logging.logFile.filename", std::string() ),
							boost::filesystem::path( file ).branch_path() ).string();
			std::string s = pt.get<std::string>( "logging.logFile.level", "ERROR" );
			if ( ( logFileLogLevel = LogLevel::str2LogLevel( s )) == LogLevel::_SMERP_UNDEFINED )	{
				errMsg_ = "unknown log level \"";
				errMsg_ += s;
				errMsg_ += " for stderr";
				return false;
			}
		}
		else
			logToFile = false;

		if ( pt.get_child_optional( "logging.syslog" ))	{
			logToSyslog = true;
			std::string s = pt.get<std::string>( "logging.syslog.facility", "LOCAL4" );
			if ( ( syslogFacility = SyslogFacility::str2SyslogFacility( s )) == SyslogFacility::_SMERP_SYSLOG_FACILITY_UNDEFINED )	{
				errMsg_ = "unknown syslog facility \"";
				errMsg_ += s;
				errMsg_ += "\"";
				return false;
			}
			s = pt.get<std::string>( "logging.syslog.level", "NOTICE" );
			if ( ( syslogLogLevel = LogLevel::str2LogLevel( s )) == LogLevel::_SMERP_UNDEFINED )	{
				errMsg_ = "unknown log level \"";
				errMsg_ += s;
				errMsg_ += " for syslog";
				return false;
			}
		}
		else
			logToSyslog = false;

		if ( pt.get_child_optional( "logging.eventlog" )) {
			logToEventlog = true;
			eventlogLogName = pt.get<std::string>( "logging.eventlog.name", "smerpd" );
			eventlogSource = pt.get<std::string>( "logging.eventlog.source", "unknown" );
			std::string s = pt.get<std::string>( "logging.eventlog.level", "NOTICE" );
			if ( ( eventlogLogLevel = LogLevel::str2LogLevel( s )) == LogLevel::_SMERP_UNDEFINED )	{
				errMsg_ = "unknown log level \"";
				errMsg_ += s;
				errMsg_ += " for Event Log";
				return false;
			}
		}
		else
			logToEventlog = false;

		return true;
	}

} // namespace _SMERP
