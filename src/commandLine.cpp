//
// commandLine.cpp
//

#include "commandLine.hpp"

#include <boost/program_options.hpp>
#include <string>

namespace prgOpts = boost::program_options;

namespace _SMERP {

	static const unsigned short DEFAULT_DEBUG_LEVEL = 2;

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
#endif
// Options
				( "foreground,f", "run in foreground (logs only on stderr)" )
				( "config-file,c", prgOpts::value<std::string>(), "configuration file" )
				( "debug,d", prgOpts::value<std::string>(), "set debug level (to be used only with --foreground)" )
#if !defined(_WIN32)
// Unix daemon options
				( "user,u", prgOpts::value<std::string>(), "run as <user>" )
				( "group,g", prgOpts::value<std::string>(), "run as <group>" )
#endif
				;
		command = DEFAULT;
		foreground = false;
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
					errMsg_ = "-v (--version) can not be specified together with -h|-p|-t|-T";
					return false;
				}
			}

			if ( clMap.count( "help" ))	{
				if ( command == DEFAULT )
					command = PRINT_HELP;
				else	{
					errMsg_ = "-h (--help) can not be specified together with -v|-p|-t|-T";
					return false;
				}
			}
			if ( clMap.count( "print-config" ))	{
				if ( command == DEFAULT )
					command = PRINT_CONFIG;
				else	{
					errMsg_ = "-p (--print-config) can not be specified together with -v|-h|-t|-T";
					return false;
				}
			}
			if ( clMap.count( "check-config" ))	{
				if ( command == DEFAULT )
					command = CHECK_CONFIG;
				else	{
					errMsg_ = "-t (--check-config) can not be specified together with -v|-h|-p|-T";
					return false;
				}
			}
			if ( clMap.count( "test-config" ))	{
				if ( command == DEFAULT )
					command = TEST_CONFIG;
				else	{
					errMsg_ = "-t (--test-config) can not be specified together with -v|-h|-p|-t";
					return false;
				}
			}

#if defined(_WIN32)
			if ( clMap.count( "install" ))	{
				if ( command == DEFAULT )
					command = INSTALL_SERVICE;
				else	{
					errMsg_ = "--install can not be specified together with -h|-p|-t|-T";
					return false;
				}
			}

			if ( clMap.count( "remove" ))	{
				if ( command == DEFAULT )
					command = REMOVE_SERVICE;
				else	{
					errMsg_ = "--remove can not be specified together with -h|-p|-t|-T";
					return false;
				}
			}			
#endif

			if ( clMap.count( "foreground" ))
				foreground = true;

			if ( clMap.count( "debug" ))
				debugLevel = clMap["debug"].as<std::string>();

			if ( clMap.count( "config-file" ))
				cfgFile = clMap["config-file"].as<std::string>();

#if !defined(_WIN32)
			if ( clMap.count( "user" ))
				user = clMap["user"].as<std::string>();
			if ( clMap.count( "group" ))
				group = clMap["group"].as<std::string>();
#endif

			return true;

		}
		catch( std::exception& e )	{
			errMsg_ = "BLA BLA:", errMsg_ += e.what();
			return false;
		}
	}

} // namespace _SMERP

