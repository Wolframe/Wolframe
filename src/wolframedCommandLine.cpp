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
// wolframedCommandLine.cpp
//

#include "wolframedCommandLine.hpp"

#include <boost/program_options.hpp>
#include <string>

#include <boost/algorithm/string.hpp>

namespace prgOpts = boost::program_options;

namespace _Wolframe {
namespace config {

// Aba: if we set the default level INFO, we can't react later in the code..
// The console logger has a default level, which is overwritten by the config
static const log::LogLevel::Level DEFAULT_DEBUG_LEVEL = log::LogLevel::LOGLEVEL_UNDEFINED;

CmdLineConfig::CmdLineConfig()
{
	options_.add_options()
			// Commands
			( "version,v", "print version" )
			( "help,h", "print help message" )
			( "print-config,p", "print configuration" )
			( "check-config,t", "check the configuration" )
			( "test-config,T", "test the configuration" )
#if defined(_WIN32)
			// Windows Service Commands
			( "install", "install as Windows service" )
			( "remove", "remove registered Windows service" )
			( "service", "run the service (don't call directly!)" )
#endif
			// Options
#if !defined(_WIN32)
			( "foreground,f", "run in foreground (logs only on stderr)" )
#endif
			( "config-file,c", prgOpts::value<std::string>(), "configuration file" )
			( "config-xml", "force configuration file parsing to xml" )
			( "config-info", "force configuration file parsing to info" )
#if !defined(_WIN32)
			( "debug,d", prgOpts::value<std::string>(), "set debug level (active only with --foreground)" )
#else
			( "debug,d", prgOpts::value<std::string>(), "set debug level" )
#endif
#if !defined(_WIN32)
			( "use-config-logging", "log according to the configuration file (active only with --foreground)" )
			// Unix daemon options
			( "user,u", prgOpts::value<std::string>(), "run as <user>" )
			( "group,g", prgOpts::value<std::string>(), "run as <group>" )
			( "pidfile", prgOpts::value<std::string>(), "use daemon pidfile <pidfile>" )
#endif
			;
	command = DEFAULT;
#if !defined(_WIN32)
	foreground = false;
#endif
	cfgType = ApplicationConfiguration::CONFIG_UNDEFINED;
	useLogConfig = false;
	debugLevel = DEFAULT_DEBUG_LEVEL;
}

bool CmdLineConfig::parse( int argc, char* argv[] )
{
	try	{
		prgOpts::variables_map	clMap;

		store( prgOpts::parse_command_line( argc, argv, options_ ), clMap );
		notify( clMap );

		if ( clMap.count( "version" ))	{
			if ( command == DEFAULT )
				command = PRINT_VERSION;
			else	{
				errMsg_ = "-v (--version) cannot be specified together with -h|-p|-t|-T";
				return false;
			}
		}

		if ( clMap.count( "help" ))	{
			if ( command == DEFAULT )
				command = PRINT_HELP;
			else	{
				errMsg_ = "-h (--help) cannot be specified together with -v|-p|-t|-T";
				return false;
			}
		}
		if ( clMap.count( "print-config" ))	{
			if ( command == DEFAULT )
				command = PRINT_CONFIG;
			else	{
				errMsg_ = "-p (--print-config) cannot be specified together with -v|-h|-t|-T";
				return false;
			}
		}
		if ( clMap.count( "check-config" ))	{
			if ( command == DEFAULT )
				command = CHECK_CONFIG;
			else	{
				errMsg_ = "-t (--check-config) cannot be specified together with -v|-h|-p|-T";
				return false;
			}
		}
		if ( clMap.count( "test-config" ))	{
			if ( command == DEFAULT )
				command = TEST_CONFIG;
			else	{
				errMsg_ = "-T (--test-config) cannot be specified together with -v|-h|-p|-t";
				return false;
			}
		}

#if defined(_WIN32)
		if ( clMap.count( "install" ))	{
			if ( command == DEFAULT )
				command = INSTALL_SERVICE;
			else	{
				errMsg_ = "--install cannot be specified together with -v|-h|-p|-t|-T";
				return false;
			}
		}

		if ( clMap.count( "remove" ))	{
			if ( command == DEFAULT )
				command = REMOVE_SERVICE;
			else	{
				errMsg_ = "--remove cannot be specified together with -v|-h|-p|-t|-T";
				return false;
			}
		}

		if ( clMap.count( "service" ))	{
			if ( command == DEFAULT )
				command = RUN_SERVICE;
			else	{
				errMsg_ = "--service cannot be specified together with -v|-h|-p|-t|-T";
				return false;
			}
		}
#endif

#if !defined(_WIN32)
		if ( clMap.count( "foreground" ))	{
			foreground = true;
			switch( command )	{
				case PRINT_HELP:
				case PRINT_VERSION:
				case CHECK_CONFIG:
				case TEST_CONFIG:
				case PRINT_CONFIG:
					errMsg_ = "-f (--foreground) makes no sense together with -v|-h|-p|-t|-T";
					break;
				case DEFAULT:
					break;
				default: throw std::logic_error( "'command' in command line parser has an unknwn value" );
			}
		}
#endif
		if ( clMap.count( "use-config-logging" ))
			useLogConfig = true;

		if ( clMap.count( "config-xml" ))	{
			if ( cfgType == ApplicationConfiguration::CONFIG_UNDEFINED ||
					cfgType == ApplicationConfiguration::CONFIG_XML )
				cfgType = ApplicationConfiguration::CONFIG_XML;
			else	{
				errMsg_ = "configuration file cannot be forced to more than one type";
				return false;
			}
		}

		if ( clMap.count( "config-info" ))	{
			if ( cfgType == ApplicationConfiguration::CONFIG_UNDEFINED ||
					cfgType == ApplicationConfiguration::CONFIG_INFO )
				cfgType = ApplicationConfiguration::CONFIG_INFO;
			else	{
				errMsg_ = "configuration file cannot be forced to more than one type";
				return false;
			}
		}

		if ( clMap.count( "debug" ))	{
			std::string str = clMap["debug"].as<std::string>();
			std::string s = str;
			boost::trim( s );
			boost::to_upper( s );
			debugLevel = log::LogLevel::strToLogLevel( s );
			if ( debugLevel == log::LogLevel::LOGLEVEL_UNDEFINED )	{
				errMsg_ = "invalid debug level '";
				errMsg_ += str,	errMsg_ += "'";
				return false;
			}
		}

		if ( clMap.count( "config-file" ))
			cfgFile = clMap["config-file"].as<std::string>();

#if !defined(_WIN32)
		if ( clMap.count( "user" ))
			user = clMap["user"].as<std::string>();
		if ( clMap.count( "group" ))
			group = clMap["group"].as<std::string>();
		if ( clMap.count( "pidfile" ))
			pidFile = clMap["pidfile"].as<std::string>();
#endif

		return true;
	}
	catch( std::exception& e )	{
		errMsg_ = e.what();
		return false;
	}
}

}} // namespace _Wolframe::config

