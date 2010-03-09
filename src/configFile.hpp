#ifndef _CONFIGFILE_HPP_INCLUDED
#define _CONFIGFILE_HPP_INCLUDED

#include <string>

namespace _SMERP	{

	struct CfgFileConfig	{
		std::string	user;
		std::string	group;

		std::string	serverAddress;
		unsigned short	port;
				// Logger configuration
		std::string	logFile;
//		severityLevel_t	fileLogLevel;
		std::string	facility;
//		severityLevel_t	syslogLogLevel;
//		severityLevel_t	stderrLogLevel;

	};

}

#endif // _CONFIGFILE_HPP_INCLUDED
