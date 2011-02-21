#ifndef _CONFIGFILE_HPP_INCLUDED
#define _CONFIGFILE_HPP_INCLUDED

#include "logLevel.hpp"
#include "logSyslogFacility.hpp"
#include "serverEndpoint.hpp"

#include "appConfig.hpp"

#include <string>
#include <list>

namespace _SMERP	{

	struct CfgFileConfig	{
		std::string		file;

// server configuration
		Configuration::ServerConfiguration	*srvConfig;
// database configuration
		Configuration::DatabaseConfiguration	*dbConfig;
// logger configuration
		Configuration::LoggerConfiguration	*logConfig;

	private:
		std::string		errMsg_;

	public:
		static const char* chooseFile( const char *globalFile,
						const char *userFile,
						const char *localFile );

		bool parse( const char *filename );
		std::string errMsg( void )		{ return errMsg_; }
	};

} // namespace _SMERP

#endif // _CONFIGFILE_HPP_INCLUDED
