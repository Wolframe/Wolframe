/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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

#include "config/configurationTree.hpp"
#include "config/valueParser.hpp"
#include "appConfig.hpp"
#include "module/moduleDirectory.hpp"
#include "utils/fileUtils.hpp"
#include "wolframedCommandLine.hpp"
#include "standardConfigs.hpp"		// fuck-up - idiotic interaction with ...
#include "logger-v1.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include <boost/algorithm/string.hpp>
#include <string>
#include <ostream>
#include <stdexcept>

#define DO_STRINGIFY(x)	#x
#define STRINGIFY(x)	DO_STRINGIFY(x)


using namespace _Wolframe::utils;
using namespace boost::filesystem;

const char* MODULE_SECTION = "LoadModules";
const char* MODULE_SECTION_MSG = "Module list:";

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


bool ApplicationConfiguration::addConfig( const std::string& nodeName,
					  ConfigurationBase* conf )
{
	std::string nodeNameLC = nodeName;
	boost::algorithm::to_lower( nodeNameLC );
	// check if the label already exists
	if ( m_section.find( nodeNameLC ) != m_section.end() )
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
	m_section[ nodeNameLC ] = pos;

	return true;
}


ApplicationConfiguration::ConfigFileType ApplicationConfiguration::fileType ( const char *filename, ConfigFileType type )
{
	std::string file = utils::resolvePath( boost::filesystem::absolute( filename ).string() );
	if ( !boost::filesystem::exists( file ))	{
		LOG_FATAL << "Configuration file " << file << " does not exist.";
		return CONFIG_UNDEFINED;
	}

	try	{
		switch ( type )	{
			case CONFIG_INFO:
				LOG_TRACE << "Forced configuration file type: INFO";
				return CONFIG_INFO;

			case CONFIG_XML:
				LOG_TRACE << "Forced configuration file type: XML";
				return CONFIG_XML;

			case CONFIG_UNDEFINED:	{
				try
				{
					utils::FileType ftype = utils::getFileType( filename);
					switch (ftype.format)
					{
						case utils::FileType::XML:
						{
							return CONFIG_XML;
						}
						case utils::FileType::Info:
						{
							return CONFIG_INFO;
						}
						case utils::FileType::Unknown:
						{
							if (ftype.encoding == utils::FileType::UCS1)
							{
								LOG_FATAL << "Cannot guess configuration file type: " << filename;
							}
							else
							{
								LOG_FATAL << "Cannot guess configuration file type (it is neither an info format file in UTF-8 encoding nor an XML): " << filename;
							}
							return CONFIG_UNDEFINED;
						}
					}
				}
				catch (const std::exception& e)
				{
					LOG_FATAL << "Cannot guess configuration file type of '" << filename << "' because of an error: '" << e.what() << "'";
					return CONFIG_UNDEFINED;
				}
				break;
			}
			default:
				LOG_FATAL << "Invalid configuration file type for '" << file << "'";
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
	configFile = utils::resolvePath( boost::filesystem::absolute( filename ).string() );
	assert( boost::filesystem::exists( configFile ));

	try	{
		types::PropertyTree pt;
		bool isXML = false;

		if (type == CONFIG_INFO)
		{
			pt = utils::readInfoPropertyTreeFile( configFile);
		}
		else if (type == CONFIG_XML)
		{
			isXML = true;
			pt = utils::readXmlPropertyTreeFile( configFile);
		}
		else
		{
			isXML = (utils::getFileType( configFile).format == utils::FileType::XML);
			pt = utils::readPropertyTreeFile( configFile);
		}
		types::PropertyTree::Node ptnode = pt.root();

		bool retVal = true;
		for ( types::PropertyTree::Node::const_iterator it = pt.begin(); it != pt.end(); it++ )	{
			LOG_TRACE << "Parse list of modules : parsing root element '" << it->first << "'";
			if ( isXML && it->first == "<xmlcomment>" )
				continue;
			if ( boost::algorithm::iequals( it->first, MODULE_SECTION ))	{
				// Parse LoadModules section:
				for ( types::PropertyTree::Node::const_iterator L2it = it->second.begin();
										L2it != it->second.end(); L2it++ )	{
					if ( boost::algorithm::iequals( L2it->first, "module" ))	{
						std::string modFile;
						if ( !Parser::getValue( MODULE_SECTION_MSG, *L2it, modFile ))	{
							retVal = false;
						}
						else	{
							// check for duplicates
							bool isDuplicate = false;
							for ( std::list< std::string >::const_iterator Vit = m_modFiles.begin();
													Vit != m_modFiles.end(); Vit++ )	{
								if ( boost::algorithm::iequals( *Vit, modFile ))	{
									LOG_WARNING << MODULE_SECTION_MSG << "duplicate module file: '"
										    << modFile << "'. Ignoring the second request.";
									isDuplicate = true;
								}
							}
							if ( ! isDuplicate )
								m_modFiles.push_back( modFile );
						}
					}
					else if ( boost::algorithm::iequals( L2it->first, "directory" ))	{
						bool isDefined = ( ! m_modFolder.empty());
						if ( ! config::Parser::getValue( MODULE_SECTION_MSG, *L2it, m_modFolder, &isDefined ))
							retVal = false;
					}
					else	{
						LOG_WARNING << MODULE_SECTION_MSG << " unknown configuration option: '"
							    << L2it->first << "'";
					}
				}
				// Resolve relative module pathes:
				if (!m_modDir) throw std::runtime_error( "Module directory structure not defined");
				std::list< std::string >::iterator pi = m_modFiles.begin(), pe = m_modFiles.end();
				for (; pi != pe; ++pi)
				{
					if (pi->empty())
					{
						LOG_ERROR << MODULE_SECTION_MSG << "Configured empty module name";
						continue;
					}
					boost::filesystem::path modfile( *pi);
					if (modfile.is_absolute())
					{
						LOG_NOTICE << MODULE_SECTION_MSG << "Module configured has absolute module path '" << *pi << "'";
					}
					else
					{
						std::string modpath = m_modDir->getAbsoluteModulePath( *pi, m_modFolder);
						LOG_NOTICE << MODULE_SECTION_MSG << "Resolved relative module path '" << *pi << "' as '" << modpath << "'";
						*pi = modpath;
					}
					LOG_TRACE << MODULE_SECTION_MSG << "Added module file '" << *pi << "'";
				}
			}
		}
		LOG_TRACE << "Configuration: parsing modules list finished " << (retVal ? "OK" : "with errors");

		return retVal;
	}
	catch( std::exception& e )	{
		LOG_FATAL << "Parsing configuration: " << e.what();
		return false;
	}
}

bool ApplicationConfiguration::parse ( const char *filename, ConfigFileType type )
{
	configFile = utils::resolvePath( boost::filesystem::absolute( filename ).string() );
	assert( boost::filesystem::exists( configFile ));
	types::PropertyTree pt;

	try	{
		bool isXML = false;
		if (type == CONFIG_INFO)
		{
			pt = utils::readInfoPropertyTreeFile( configFile);
		}
		else if (type == CONFIG_XML)
		{
			isXML = true;
			pt = utils::readXmlPropertyTreeFile( configFile);
		}
		else
		{
			isXML = (utils::getFileType( configFile).format == utils::FileType::XML);
			pt = utils::readPropertyTreeFile( configFile);
		}
		types::PropertyTree::Node ptnode = pt.root();

		bool retVal = true;
		for ( types::PropertyTree::Node::const_iterator it = pt.begin(); it != pt.end(); it++ )	{
			LOG_TRACE << "Configuration : parsing root element '" << it->first << "'";
			if ( isXML && it->first == "<xmlcomment>" )
				continue;
			// skip modules to load
			if ( boost::algorithm::iequals( it->first, MODULE_SECTION ))
				continue;
			std::map< std::string, std::size_t >::iterator confIt;
			std::string nodeNameLC = it->first;
			boost::algorithm::to_lower( nodeNameLC );
			if (( confIt = m_section.find( nodeNameLC ) ) != m_section.end() )	{
				if ( ! (m_conf[confIt->second])->parse( config::ConfigurationNode( it->second ),
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
		m_conf[i]->setCanonicalPathes( boost::filesystem::path( configFile ).branch_path().string() );
}

void ApplicationConfiguration::finalize()
{
	foreground = true;
	for( std::size_t i = 0; i <  m_conf.size(); i++ )
	{
		m_conf[i]->setCanonicalPathes( boost::filesystem::path( configFile ).branch_path().string() );
	}
}

void ApplicationConfiguration::print( std::ostream& os ) const
{

	os << "Configuration file: " << configFile << std::endl;
	os << "Configuration file type: " << (m_type == CONFIG_INFO ? "info" : "XML") << std::endl;
	// Unix daemon
#if !defined(_WIN32)
	os << "Run in foreground: " << (foreground ? "yes" : "no") << std::endl;
#endif
	// modules
	if ( ! m_modFiles.empty() )	{
		os << "Default module directory: " << (m_modFolder.empty() ? "(none)" : m_modFolder) << std::endl;
		os << "Module files to load:" << std::endl;
		for ( std::list< std::string >::const_iterator it = m_modFiles.begin();
								it != m_modFiles.end(); it++ )
			os << "   " << *it << std::endl;
	}
	// rest of the configuration
	for ( std::size_t i = 0; i < m_conf.size(); i++ )	{
		os << std::endl;
		m_conf[ i ]->print( os, 0 );
	}
}

/// Check if the application configuration makes sense
bool ApplicationConfiguration::check() const
{
	bool retVal = true;
	// check the list of modules
	for ( std::list< std::string >::const_iterator it1 = m_modFiles.begin();
						      it1 != m_modFiles.end(); it1++ )	{
		std::list< std::string >::const_iterator it2 = it1;
		it2++;
		for ( ; it2 != m_modFiles.end(); it2++ )	{
			if ( boost::algorithm::iequals( *it1, *it2 ))	{
				LOG_ERROR << "duplicate module file: '" << *it1 << "'";
				retVal = false;
			}
		}
		if ( it1->empty() )
			throw  std::logic_error( "empty module file name encountered" );
	}

	// check the rest of the configuration
	for ( std::size_t i = 0; i < m_conf.size(); i++ )
		if ( ! m_conf[ i ]->check())
			return false;
	return retVal;
}

}} // namespace _Wolframe::config
