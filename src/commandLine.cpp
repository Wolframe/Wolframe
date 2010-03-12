//
// commandLine.cpp
//

#include "commandLine.hpp"

#include <boost/program_options.hpp>

namespace _SMERP {

	CmdLineConfig::CmdLineConfig( int argc, char *argv[] )
	{
//	// Command line options
//	boost::program_options::options_description cmdLineOptions( "Command line options" );
//	cmdLineOptions.add_options()
//	// Commands
//			( "version,v", "print version" )
//			( "help,h", "print help message" )
//			( "print-config,p", "print configuration" )
//			( "check-config,t", "check the configuration" )
//			( "test-config,T", "check and test the configuration" )
//	// Options
//			( "foreground,f", "run in foreground (logs only on stderr)" )
//			( "config-file,c", po::value<string>(&configFile), "configuration file" )
//			( "debug,d", po::value<int>(&debugLevel), "set debug level (to be used only with --foreground)" )
//			( "user,u", po::value<string>(&userName), "run as <user>" )
//			( "group,g", po::value<string>(&groupName), "run as <group>" )
//			;

//	po::variables_map	clMap;
//
//	store( po::parse_command_line( argc, argv, cmdLineOpts ), clMap );
//	notify( clMap );
	}

}

