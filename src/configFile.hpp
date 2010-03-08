#ifndef _CONFIGFILE_HPP_INCLUDED
#define _CONFIGFILE_HPP_INCLUDED

#include <string>

namespace _SMERP	{

	struct CfgFileConfig	{
		std::string	user;
		std::string	group;
		std::string	serverAddress;
		unsigned short	port;
		bool		foreground;
		unsigned short	logLevel;
	};

}

#endif // _CONFIGFILE_HPP_INCLUDED
