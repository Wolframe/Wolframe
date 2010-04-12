//
// configFile.cpp
//

#include "configFile.hpp"

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <vector>
#include <string>

#if defined(_WIN32)		// we are on Windows
#include <string.h>
#define strcasecmp(a, b)	_stricmp((a), (b))
#else
#include <strings.h>
#endif


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
		ptree pt;

		// Load the XML file into the property tree. If reading fails
		// (cannot open file, parse error), an exception is thrown.
		read_xml( filename, pt );

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
				address.push_back( make_pair( tmpStr, port ));
			}
			else if ( v.first == "SSLsocket" )	{
				SSLaddress.push_back( make_pair( tmpStr, port ));
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

		SSLcertificate = boost::filesystem::complete(
							pt.get<std::string>( "server.SSL.certificate", std::string() ),
							boost::filesystem::path( file ).branch_path() ).string();
		SSLkey = boost::filesystem::complete(
							pt.get<std::string>( "server.SSL.key", std::string() ),
							boost::filesystem::path( file ).branch_path() ).string();
		SSLCAdirectory = boost::filesystem::complete(
							pt.get<std::string>( "server.SSL.CAdirectory", std::string() ),
							boost::filesystem::path( file ).branch_path() ).string();
		SSLCAchainFile = boost::filesystem::complete(
							pt.get<std::string>( "server.SSL.CAchainFile", std::string() ),
							boost::filesystem::path( file ).branch_path() ).string();		tmpStr = pt.get<std::string>( "server.SSL.verify", std::string() );
		if ( strcasecmp( tmpStr.c_str(), "no" )	&& strcasecmp( tmpStr.c_str(), "false" ) && strcasecmp( tmpStr.c_str(), "0" ))
			SSLverify = true;
		else
			SSLverify = false;

		dbHost = pt.get<std::string>( "database.host", std::string() );
		dbPort = pt.get<unsigned short>( "database.port", 0 );
		dbName = pt.get<std::string>( "database.name", std::string() );
		dbUser = pt.get<std::string>( "database.user", std::string() );
		dbPassword = pt.get<std::string>( "database.password", std::string() );

		if ( pt.get_child_optional( "logging.stderr" ))	{
			logToStderr = true;
			stderrLogLevel = pt.get<std::string>( "logging.stderr.level", "NOTICE" );
		}
		else
			logToStderr = false;
		if ( pt.get_child_optional( "logging.logFile" ))	{
			logToFile = true;
			logFile = boost::filesystem::complete(
						pt.get<std::string>( "logging.logFile.filename", std::string() ),
							boost::filesystem::path( file ).branch_path() ).string();
			logFileLogLevel = pt.get<std::string>( "logging.logFile.level", "ERROR" );
		}
		else
			logToFile = false;
		if ( pt.get_child_optional( "logging.syslog" ))	{
			logToSyslog = true;
			syslogFacility = pt.get<std::string>( "logging.syslog.facility", "LOCAL4" );
			syslogLogLevel = pt.get<std::string>( "logging.syslog.level", "NOTICE" );
		}
		else
			logToSyslog = false;
		if ( pt.get_child_optional( "logging.eventlog" )) {
			logToEventlog = true;
			eventlogLogName = pt.get<std::string>( "logging.eventlog.name", "smerpd" );
			eventlogSource = pt.get<std::string>( "logging.eventlog.source", "unknown" );
			eventlogLogLevel = pt.get<std::string>( "logging.eventlog.level", "NOTICE" );
		}
		else
			logToEventlog = false;

		return true;
	}

} // namespace _SMERP
