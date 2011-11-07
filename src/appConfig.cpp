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

#include "config/ConfigurationTree.hpp"
#include "appConfig.hpp"
#include "commandLine.hpp"
#include "standardConfigs.hpp"		// fuck-up - idiotic interaction with ...
#include "logger-v1.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>
#include <stdexcept>

const char* MODULE_SECTION = "LoadModules";

namespace _Wolframe {
namespace config {

static bool plausibleConfig( boost::property_tree::ptree& pt )
{
	size_t lvl1, lvl2;
	lvl1 = lvl2 = 0;
	for ( boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++, lvl1++ )
		lvl2 += it->second.size();
	if ( lvl1 >= lvl2 || lvl1 == 0 )
		return false;
	return true;
}

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


bool ApplicationConfiguration::addConfig( const std::string& nodeName,
					  ConfigurationBase* conf )
{
	// check if the label already exists
	if ( m_section.find( nodeName ) != m_section.end() )
		return false;

	// find the appropriate index in the configurations vector
	std::vector< ConfigurationBase* >::const_iterator it = m_conf.begin();
	std::size_t pos = 0;
	bool found = false;
	for( ; it != m_conf.end(); it++, pos++ )	{
		if ( *it == conf )	{
			found = true;
			break;
		}
	}
	if ( !found )	{
		pos = m_conf.size();
		m_conf.push_back( conf );
	}
	m_section[ nodeName ] = pos;

	return true;
}


ApplicationConfiguration::ConfigFileType ApplicationConfiguration::fileType ( const char *filename, ConfigFileType type )
{
	std::string file = resolvePath( boost::filesystem::absolute( filename ).string() );
	if ( !boost::filesystem::exists( file ))	{
		LOG_FATAL << "Configuration file " << file << " does not exist.";
		return CONFIG_UNDEFINED;
	}

	boost::property_tree::ptree	pt;
	try	{
		switch ( type )	{
			case CONFIG_INFO:
				LOG_TRACE << "Forced configuration file type: INFO";
				return CONFIG_INFO;

			case CONFIG_XML:
				LOG_TRACE << "Forced configuration file type: XML";
				return CONFIG_XML;

			case CONFIG_UNDEFINED:	{
				try	{
					read_xml( file, pt, boost::property_tree::xml_parser::no_comments |
						  boost::property_tree::xml_parser::trim_whitespace );
					pt = pt.get_child( "configuration" );
					if ( plausibleConfig( pt ) )	{
						LOG_TRACE << "Guessed configuration file type: XML";
						return CONFIG_XML;
					}
					else	{
						LOG_FATAL << "Cannot guess configuration file type: " << file;
						return CONFIG_UNDEFINED;
					}
				}
				catch( boost::property_tree::xml_parser::xml_parser_error )	{
					try	{
						read_info( file, pt );
						if ( plausibleConfig( pt ) )	{
							LOG_TRACE << "Guessed configuration file type: INFO";
							return CONFIG_INFO;
						}
						else	{
							LOG_FATAL << "Cannot guess configuration file type or invalid XML: " << filename;
							return CONFIG_UNDEFINED;
						}
					}
					catch( boost::property_tree::info_parser::info_parser_error& )	{
						LOG_FATAL << "Cannot guess configuration file type or invalid XML: " << filename;
						return CONFIG_UNDEFINED;
					}
				}
				catch( boost::property_tree::ptree_bad_path )	{
					LOG_FATAL << "Invalid XML configuration file: " << file;
					return CONFIG_UNDEFINED;
				}
				break;
			}
			default:
				LOG_FATAL << "Invalid XML configuration file: " << file;
		}
		return CONFIG_UNDEFINED;
	}
	catch( std::exception& e )	{
		LOG_ERROR << "Parsing configuration: " << e.what();
		return CONFIG_UNDEFINED;
	}
}


bool ApplicationConfiguration::parseModules ( const char *filename, ConfigFileType type )
{
	m_type = type;
	configFile = resolvePath( boost::filesystem::absolute( filename ).string() );
	assert( boost::filesystem::exists( configFile ));

	// Create an empty property tree object
	boost::property_tree::ptree	pt;
	try	{
		switch ( type )	{
			case CONFIG_INFO:
				read_info( configFile, pt );
				break;
			case CONFIG_XML:
				read_xml( configFile, pt, boost::property_tree::xml_parser::no_comments |
					  boost::property_tree::xml_parser::trim_whitespace );
				pt = pt.get_child( "configuration" );
				break;
			case CONFIG_UNDEFINED:
				throw std::logic_error( "undefined configuration file type" );
		}

		bool retVal = true;
		for ( boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++ )	{
			LOG_TRACE << "Parse list of modules : parsing root element '" << it->first << "'";
			if ( it->first == "<xmlcomment>" && m_type == CONFIG_XML )
				continue;
			if ( boost::algorithm::iequals( it->first, MODULE_SECTION ))	{
			}
		}
		LOG_TRACE << "Configuration : parsing modules list finished " << (retVal ? "OK" : "with errors");
		return retVal;
	}
	catch( std::exception& e )	{
		LOG_FATAL << "Parsing configuration: " << e.what();
		return false;
	}
}

bool ApplicationConfiguration::parse ( const char *filename, ConfigFileType type )
{
	configFile = resolvePath( boost::filesystem::absolute( filename ).string() );
	assert( boost::filesystem::exists( configFile ));

	// Create an empty property tree object
	boost::property_tree::ptree	pt;
	try	{
		switch ( type )	{
			case CONFIG_INFO:
				read_info( configFile, pt );
				break;
			case CONFIG_XML:
				read_xml( configFile, pt, boost::property_tree::xml_parser::no_comments |
					  boost::property_tree::xml_parser::trim_whitespace );
				pt = pt.get_child( "configuration" );
				break;
			case CONFIG_UNDEFINED:
				throw std::logic_error( "undefined configuration file type" );
		}

		bool retVal = true;
		for ( boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++ )	{
			LOG_TRACE << "Configuration : parsing root element '" << it->first << "'";
			if ( it->first == "<xmlcomment>" && m_type == CONFIG_XML )
				continue;
			if ( boost::algorithm::iequals( it->first, MODULE_SECTION ))
				continue;
			std::map< std::string, std::size_t >::iterator confIt;
			if (( confIt = m_section.find( it->first ) ) != m_section.end() )	{
				if ( ! (m_conf[confIt->second])->parse( config::ConfigurationTree( it->second ),
									confIt->first, m_modDir ))
					retVal = false;
			}
			else	{
				LOG_WARNING << "configuration root: Unknown configuration option '"
					    << it->first << "'";
			}
		}
		LOG_TRACE << "Configuration : parsing finished " << (retVal ? "OK" : "with errors");
		return retVal;
	}
	catch( std::exception& e )	{
		LOG_FATAL << "Parsing configuration: " << e.what();
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
		loggerCfg->foreground( cmdLine.debugLevel, cmdLine.useLogConfig );
#if !defined(_WIN32)
	serviceCfg->override( cmdLine.user, cmdLine.group, cmdLine.pidFile );
#endif
	for( std::size_t i = 0; i <  m_conf.size(); i++ )
		m_conf[i]->setCanonicalPathes( configFile );
}


void ApplicationConfiguration::print( std::ostream& os ) const
{

	os << "Configuration file: " << configFile << std::endl;
	os << "Configuration file type: " << (m_type == CONFIG_INFO ? "info" : "xml") << std::endl;
	// Unix daemon
#if !defined(_WIN32)
	os << "Run in foreground: " << (foreground ? "yes" : "no") << std::endl;
#endif
	for ( std::size_t i = 0; i < m_conf.size(); i++ )	{
		os << std::endl;
		m_conf[ i ]->print( os, 0 );
	}
}

/// Check if the application configuration makes sense
bool ApplicationConfiguration::check() const
{
	for ( std::size_t i = 0; i < m_conf.size(); i++ )
		if ( ! m_conf[ i ]->check())
			return false;
	return true;
}

}} // namespace _Wolframe::config
