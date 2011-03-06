//
// appConfig.cpp
//

#include "appConfig.hpp"
#include "commandLine.hpp"
#include "standardConfigs.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>


namespace _Wolframe {
	namespace Configuration {

const char* ApplicationConfiguration::chooseFile( const char *globalFile, const char *userFile, const char *localFile )
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


bool ApplicationConfiguration::addConfig( const std::string& nodeName, ConfigurationBase *config )
{
	// check if the label already exists
	if ( section_.find( nodeName ) != section_.end() )
		return false;

	// find the appropriate index in the configurations vector
	std::vector< ConfigurationBase* >::const_iterator it = conf_.begin();
	std::size_t pos = 0;
	bool found = false;
	for( ; it != conf_.end(); it++, pos++ )	{
		if ( *it == config )	{
			found = true;
			break;
		}
	}
	if ( !found )	{
		pos = conf_.size();
		conf_.push_back( config );
	}
	section_[ nodeName ] = pos;

	return true;
}


bool ApplicationConfiguration::parse ( const char *filename, std::ostream& os )
{
	configFile = resolvePath( boost::filesystem::absolute( filename ).string() );
	if ( !boost::filesystem::exists( configFile ))	{
		os << "Configuration file " << configFile << " does not exist.";
		return false;
	}

	// Create an empty property tree object
	boost::property_tree::ptree	pt;
	try	{
		read_info( filename, pt );

		for ( boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++ )	{
			std::map< std::string, std::size_t >::iterator confIt;
			if (( confIt = section_.find( it->first ) ) != section_.end() )	{
				if ( ! conf_[ confIt->second ]->parse( it->second, it->first, os ))
					return false;
			}
			else	{
				os << "ERROR: configuration root: Unknown configuration option <" << it->first
				   << ">" << std::endl;
				return false;
			}
		}

	}
	catch( std::exception& e)	{
		os << e.what();
		return false;
	}
	return true;
}


void ApplicationConfiguration::finalize( const CmdLineConfig& cmdLine )
{
#if defined(_WIN32)
	// on Windows the user should either use -f or start in the console
	// (assuming an implicit -f) or the service option should contain
	// --service in the startup parameters of the service impling non-foreground
	if( cmdLine.command == CmdLineConfig::RUN_SERVICE )
		foreground = false;
	else
		foreground = true;
#else
	foreground = cmdLine.foreground;
#endif
	if ( foreground )
		loggerConf->foreground( cmdLine.debugLevel, cmdLine.useLogConfig );
#if !defined(_WIN32)
	serviceConf->override( cmdLine.user, cmdLine.group );
#endif
	for( std::size_t i = 0; i <  conf_.size(); i++ )
		conf_[i]->setCanonicalPathes( configFile );
}


void ApplicationConfiguration::print( std::ostream& os ) const
{

	os << "Configuration file: " << configFile << std::endl;
	// Unix daemon
#if !defined(_WIN32)
	os << "Run in foreground: " << (foreground ? "yes" : "no") << std::endl;
#endif
	for ( std::size_t i = 0; i < conf_.size(); i++ )	{
		os << std::endl;
		conf_[ i ]->print( os );
	}
}

/// Check if the application configuration makes sense
bool ApplicationConfiguration::check( std::ostream& os ) const
{
	for ( std::size_t i = 0; i < conf_.size(); i++ )
		if ( ! conf_[ i ]->check( os ))
			return false;
	return true;
}

	} // namespace Configuration
} // namespace _Wolframe
