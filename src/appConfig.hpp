//
// appConfig.hpp
//

#ifndef _APP_CONFIG_HPP_INCLUDED
#define _APP_CONFIG_HPP_INCLUDED

#include <string>


namespace _SMERP {

// server configuration structure

	struct CmdLineConfig;
	struct CfgFileConfig;

	struct ApplicationConfiguration	{
		std::string	user;
		std::string	group;
		std::string	address;
		unsigned short	port;

		unsigned	maxNoClients;
		unsigned	noThreads;
	public:
		ApplicationConfiguration( const CmdLineConfig& cmdLine, const CfgFileConfig& cfgFile );
	};
} // namespace _SMERP

#endif // _APP_CONFIG_HPP_INCLUDED
