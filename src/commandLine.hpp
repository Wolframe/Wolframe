//
// commandLine.hpp
//

#ifndef _COMMANDLINE_HPP_INCLUDED
#define _COMMANDLINE_HPP_INCLUDED

#include "logLevel.hpp"

#include <string>
#include <boost/program_options.hpp>

namespace _SMERP	{

	struct CmdLineConfig	{
		enum Command_t	{
			DEFAULT,
			PRINT_HELP,
			PRINT_VERSION,
			CHECK_CONFIG,
			TEST_CONFIG,
			PRINT_CONFIG,
#if defined(_WIN32)
			INSTALL_SERVICE,
			REMOVE_SERVICE,
			RUN_SERVICE,
#endif
			UNKNOWN
		};

		Command_t	command;
		bool		foreground;
		LogLevel::Level	debugLevel;
		std::string	user;
		std::string	group;
		std::string	cfgFile;
	private:
		std::string	errMsg_;
		boost::program_options::options_description	options_;

	public:
		CmdLineConfig();
		bool parse( int argc, char* argv[] );
		std::string errMsg( void )	{ return errMsg_; };
		void usage( std::ostream& os ) const	{ options_.print( os ); };
	};

} // namespace _SMERP

#endif // _COMMANDLINE_HPP_INCLUDED
