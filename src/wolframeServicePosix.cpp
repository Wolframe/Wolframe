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
#include "wolframeService.hpp"
#include "version.hpp"
#include "logger-v1.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;

bool WolframeService::loadConfig( int argc, char* argv[])
{
	log::LogBackend::instance().setConsoleLevel( log::LogLevel::LOGLEVEL_INFO );

	m_appInfo->version( Version( m_appProperties->applicationVersion() ));

	const char *configFile = 0;

	if ( !m_cmdLineCfg.parse( argc, argv ))	{	// there was an error parsing the command line
		LOG_ERROR << m_cmdLineCfg.errMsg();
		std::cerr << std::endl;
		m_cmdLineCfg.usage( std::cerr );
		std::cerr << std::endl;
		return false;
	}
// command line has been parsed successfully
// reset log level to the command line one, if specified
	if ( m_cmdLineCfg.debugLevel != log::LogLevel::LOGLEVEL_UNDEFINED )
		log::LogBackend::instance().setConsoleLevel( m_cmdLineCfg.debugLevel );

// if m_cmdLineCfg.errMsg() is not empty than we have a warning
	if ( !m_cmdLineCfg.errMsg().empty() )	{	// there was a warning parsing the command line
		LOG_WARNING << m_cmdLineCfg.errMsg();
	}

// if we have to print the version or the help do it and exit
	if ( m_cmdLineCfg.command == config::CmdLineConfig::PRINT_VERSION )	{
		std::cout << std::endl << m_appProperties->applicationName() << gettext( " version " )
			  << m_appInfo->version().toString() << std::endl << std::endl;
		return true;
	}
	if ( m_cmdLineCfg.command == config::CmdLineConfig::PRINT_HELP )	{
		std::cout << std::endl << m_appProperties->applicationName() << gettext( " version " )
			  << m_appInfo->version().toString() << std::endl;
		m_cmdLineCfg.usage( std::cout );
		std::cout << std::endl;
		return true;
	}

// decide what configuration file to use
	if ( !m_cmdLineCfg.cfgFile.empty() )	// if it has been specified than that's The One ! (and only)
		configFile = m_cmdLineCfg.cfgFile.c_str();
	else
		configFile = config::ApplicationConfiguration::chooseFile( m_appProperties->defaultMainConfig(),
										      m_appProperties->defaultUserConfig(),
										      m_appProperties->defaultLocalConfig() );
	if ( configFile == NULL )	{	// there is no configuration file
		LOG_FATAL << gettext ( "no configuration file found !" );
		return false;
	}
	if ( ! boost::filesystem::is_regular_file( configFile ))	{
		LOG_FATAL << "'" << configFile << "' is not a regular file";
		return false;
	}

	config::ApplicationConfiguration::ConfigFileType cfgType =
			config::ApplicationConfiguration::fileType( configFile, m_cmdLineCfg.cfgType );
	if ( cfgType == config::ApplicationConfiguration::CONFIG_UNDEFINED )
		return false;
	if ( !m_appConfig.parseModules( configFile, cfgType ))
		return false;
	if ( ! module::LoadModules( m_modDir, m_appConfig.moduleList() ))
		return false;
	m_appConfig.addModules( &m_modDir );
	if ( !m_appConfig.parse( configFile, cfgType ))
		return false;

// configuration file has been parsed successfully
// finalize the application configuration
	m_appConfig.finalize( m_cmdLineCfg);
	return true;
}



