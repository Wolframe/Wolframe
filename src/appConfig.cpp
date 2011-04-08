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
// appConfig.cpp
//

#include "appConfig.hpp"
#include "commandLine.hpp"
#include "standardConfigs.hpp"
#include "logger.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>


namespace _Wolframe {
namespace config {

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


bool ApplicationConfiguration::parse ( const char *filename )
{
	configFile = resolvePath( boost::filesystem::absolute( filename ).string() );
	if ( !boost::filesystem::exists( configFile ))	{
		LOG_FATAL << "Configuration file " << configFile << " does not exist.";
		return false;
	}

	// Create an empty property tree object
	boost::property_tree::ptree	pt;
	try	{
		read_info( filename, pt );
		bool retVal = true;

		for ( boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++ )	{
			std::map< std::string, std::size_t >::iterator confIt;
			if (( confIt = section_.find( it->first ) ) != section_.end() )	{
				if ( ! conf_[ confIt->second ]->parse( it->second, confIt->first ))
//					return false;
					retVal = false;
			}
			else	{
				LOG_WARNING << "configuration root: Unknown configuration option <"
					  << it->first << ">";
//				return false;
			}
		}
		return retVal;
	}
	catch( std::exception& e)	{
		LOG_FATAL << e.what();
		return false;
	}
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
bool ApplicationConfiguration::check() const
{
	for ( std::size_t i = 0; i < conf_.size(); i++ )
		if ( ! conf_[ i ]->check())
			return false;
	return true;
}

}} // namespace _Wolframe::config
