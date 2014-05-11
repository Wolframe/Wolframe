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
#include "logger-v1.hpp"

using namespace _Wolframe;

bool WolframeService::loadConfig( int argc, char* argv[])
{
// create initial console logger, so we see things going wrong
	_Wolframe::log::LogBackend::instance().setConsoleLevel( _Wolframe::log::LogLevel::LOGLEVEL_WARNING );

	m_appInfo = _Wolframe::ApplicationInfo::instance();
	m_appInfo.version( _Wolframe::Version( appProperties->applicationVersion() ));

	const char* configFile = NULL;

	if ( !m_cmdLineCfg.parse( argc, argv ))	{	// there was an error parsing the command line
		LOG_ERROR << m_cmdLineCfg.errMsg();
		m_cmdLineCfg.usage( std::cerr );
		std::cerr << std::endl;
		return false;
	}
// command line has been parsed successfully
// reset log level to the command line one, if specified
	if ( m_cmdLineCfg.debugLevel != _Wolframe::log::LogLevel::LOGLEVEL_UNDEFINED ) {
		_Wolframe::log::LogBackend::instance().setConsoleLevel( m_cmdLineCfg.debugLevel );
	}
// if m_cmdLineCfg.errMsg() is not empty than we have a warning
	if ( !m_cmdLineCfg.errMsg().empty() )	// there was a warning parsing the command line
		LOG_WARNING << m_cmdLineCfg.errMsg();

// if we have to print the version or the help do it and exit
	if ( m_cmdLineCfg.command == _Wolframe::config::CmdLineConfig::PRINT_VERSION )	{
		std::cout << appProperties->applicationName() << " version "
			  << appInfo.version().toString() << std::endl << std::endl;
		return true;
	}
	if ( m_cmdLineCfg.command == _Wolframe::config::CmdLineConfig::PRINT_HELP )	{
		m_cmdLineCfg.usage( std::cout );
		std::cout << std::endl;
		return true;
	}

// decide what configuration file to use
	if ( !m_cmdLineCfg.cfgFile.empty() )	// if it has been specified than that's The One ! (and only)
		configFile = m_cmdLineCfg.cfgFile.c_str();
	if ( configFile == NULL )	{	// there is no configuration file
		LOG_FATAL << "no configuration file found !";
		return false;
	}

	_Wolframe::config::ApplicationConfiguration::ConfigFileType cfgType =
			_Wolframe::config::ApplicationConfiguration::fileType( configFile, m_cmdLineCfg.cfgType );
	if ( cfgType == _Wolframe::config::ApplicationConfiguration::CONFIG_UNDEFINED )
		return false;
	if ( !m_appConfig.parseModules( configFile, cfgType ))
		return false;
	if ( ! _Wolframe::module::LoadModules( m_modDir, m_appConfig.moduleList() ))
		return false;
	m_appConfig.addModules( &modDir );
	if ( !m_appConfig.parse( configFile, cfgType ))
		return false;

// configuration file has been parsed successfully
// build the final configuration
	m_appConfig.finalize( m_cmdLineCfg );

// Check the configuration
	if ( m_cmdLineCfg.command == _Wolframe::config::CmdLineConfig::CHECK_CONFIG )	{
		if ( check() )	{
			std::cout << "Configuration OK" << std::endl << std::endl;
			return true;
		}
		else	{
			return true;
		}
	}

	if ( m_cmdLineCfg.command == _Wolframe::config::CmdLineConfig::PRINT_CONFIG )	{
		print( std::cout );
		std::cout << std::endl;
		return true;
	}

	if ( m_cmdLineCfg.command == _Wolframe::config::CmdLineConfig::TEST_CONFIG )	{
		std::cout << "Not implemented yet" << std::endl << std::endl;
		return true;
	}
}

log::LogLevel::Level WolframeService::winDebugLevel() const
{
	if ( m_cmdLineCfg.debugLevel != _Wolframe::log::LogLevel::LOGLEVEL_UNDEFINED ) {
		// if in a service the -d flag can be specified in the 'ImagePath' of the service description in order
		// to debug lowlevel via 'OutputDebugString'
		return m_cmdLineCfg.debugLevel;
	} else {
		return _Wolframe::log::LogLevel::LOGLEVEL_UNDEFINED;
	}
}

