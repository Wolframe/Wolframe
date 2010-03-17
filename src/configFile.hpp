#ifndef _CONFIGFILE_HPP_INCLUDED
#define _CONFIGFILE_HPP_INCLUDED

#include <string>
#include <vector>
#include <boost/program_options.hpp>

namespace _SMERP	{

	struct CfgFileConfig	{
		std::string	file;
// daemon configuration
		std::string	user;
		std::string	group;
// network configuration
		std::vector<std::string> address;
		unsigned short	port;
		unsigned short	SSLport;
// database configuration
		std::string	dbHost;
		unsigned short	dbPort;
		std::string	dbName;
		std::string	dbUser;
		std::string	dbPassword;
// logger configuration
		std::string	logFile;

	private:
		std::string	errMsg_;
		boost::program_options::options_description	options_;

	public:
		CfgFileConfig();
		static const char* chooseFile( const char *globalFile,
						const char *userFile,
						const char *localFile );

		bool parse( const char *filename );
		std::string errMsg( void )	{ return errMsg_; };
	};

}

#endif // _CONFIGFILE_HPP_INCLUDED
