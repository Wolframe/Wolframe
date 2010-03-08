#ifndef _SERVERCONFIG_HPP_INCLUDED
#define _SERVERCONFIG_HPP_INCLUDED

#include <string>


namespace _SMERP {

// server configuration structure

	struct cmdLineConfig;
	struct cfgFileConfig;

	struct serverConfig	{
		std::string	user;
		std::string	group;
		std::string	address;
		unsigned short	port;

		unsigned	maxNoClients;
		unsigned	noThreads;
	public:
		serverConfig& getConfig( const cmdLineConfig& cmdLine, const cfgFileConfig& cfgFile );
	};
} // namespace

#endif // _SERVERCONFIG_HPP_INCLUDED
