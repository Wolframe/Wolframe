#ifndef _COMMANDLINE_HPP_INCLUDED
#define _COMMANDLINE_HPP_INCLUDED

#include <string>

namespace _SMERP	{

	struct CmdLineConfig	{
		enum Command_t	{
			DEFAULT,
			PRINT_HELP,
			VERIFY_CONFIG,
			TEST_CONFIG,
			PRINT_CONFIG,
			PRINT_VERSION,
			UNKNOWN
		};

		Command_t	command;
		std::string	user;
		std::string	group;
		std::string	serverAddress;
		unsigned short	port;
		bool		foreground;
		unsigned short	debugLevel;

		CmdLineConfig( int argc, char* argv[] );
		void printUsage( void );
	};

} // namespace _SMERP

#endif // _COMMANDLINE_HPP_INCLUDED
