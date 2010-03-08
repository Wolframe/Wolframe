#ifndef _COMMANDLINE_HPP_INCLUDED
#define _COMMANDLINE_HPP_INCLUDED

#include <string>

namespace _SMERP	{

	struct cmdLineConfig	{
		std::string	user;
		std::string	group;
		std::string	serverAddress;
		unsigned short	port;
		bool		foreground;
		unsigned short	logLevel;
	};

}

#endif // _COMMANDLINE_HPP_INCLUDED
