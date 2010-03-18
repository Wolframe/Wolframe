//
// configFile.cpp
//

#include "configFile.hpp"

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <vector>
#include <string>

#include <iostream>


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
		file = resolvePath( boost::filesystem::system_complete( filename )).string();
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
		BOOST_FOREACH( ptree::value_type &v, pt.get_child( "server.listen" ))
			if ( v.first == "socket" )
				address.push_back( make_pair( v.second.get<std::string>( "address" ),
								v.second.get<unsigned short>( "port" )));
			else if ( v.first == "SSLsocket" )
				SSLaddress.push_back( make_pair( v.second.get<std::string>( "address" ),
								v.second.get<unsigned short>( "port" )));
//			else ERROR

		threads = pt.get<unsigned short>( "server.threads", 4 );
		maxClients = pt.get<unsigned short>( "server.maxClients", 256 );
		user = pt.get<std::string>( "server.user" );
		group = pt.get<std::string>( "server.group" );

		idleTimeout = pt.get<unsigned>( "server.timeout.idle", 900 );
		requestTimeout = pt.get<unsigned>( "server.timeout.request", 30 );
		answerTimeout = pt.get<unsigned>( "server.timeout.answer", 30 );
		processTimeout = pt.get<unsigned>( "server.timeout.process", 30 );

		SSLcertificate = pt.get<std::string>( "server.SSL.certificate" );
		SSLkey = pt.get<std::string>( "server.SSL.key" );
		SSLCAdirectory = pt.get<std::string>( "server.SSL.CAdirectory" );
		SSLCAchainFile = pt.get<std::string>( "server.SSL.CAchainFile" );
		SSLverify = pt.get<bool>( "server.SSL.verify", true );

		dbHost = pt.get<std::string>( "database.host" );
		dbPort = pt.get<unsigned short>( "database.port" );
		dbName = pt.get<std::string>( "database.name" );
		dbUser = pt.get<std::string>( "database.user" );
		dbPassword = pt.get<std::string>( "database.password" );

		return true;
	}

} // namespace _SMERP
