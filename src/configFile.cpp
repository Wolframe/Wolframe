//
// configFile.cpp
//

#include "configFile.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <vector>
#include <string>


namespace prgOpts = boost::program_options;

static boost::filesystem::path resolvePath(const boost::filesystem::path& p)
{
	boost::filesystem::path result;
	for ( boost::filesystem::path::iterator it = p.begin(); it != p.end(); ++it )	{
		if ( *it == ".." )	{
			// /a/b/.. is not necessarily /a if b is a symbolic link
			if ( boost::filesystem::is_symlink( result ) )
				result /= *it;
			// /a/b/../.. is not /a/b/.. under most circumstances
			// We can end up with ..s in our result because of symbolic links
			else if( result.filename() == ".." )
				result /= *it;
			// Otherwise it should be safe to resolve the parent
			else
				result = result.parent_path();
		}
		else if( *it == "." )	{
			// Ignore
		}
		else {
			// Just cat other path entries
			result /= *it;
		}
	}
	return result;
}


namespace _SMERP {

	const char* CfgFileConfig::chooseFile( const char *globalFile, const char *userFile, const char *localFile )
	{
		if ( globalFile != NULL )
			if ( boost::filesystem::exists( globalFile ))
				return globalFile;
		if ( userFile != NULL )
			if ( boost::filesystem::exists( userFile ))
				return userFile;
		if ( localFile != NULL )
			if ( boost::filesystem::exists( localFile ))
				return localFile;
		return NULL;
	}


	CfgFileConfig::CfgFileConfig()
	{
	// Config file options
	options_.add_options()
// daemon configuration
			( "user,u", prgOpts::value<std::string>(), "run as <user>" )
			( "group,g", prgOpts::value<std::string>(), "run as <group>" )
			( "threads", prgOpts::value<unsigned short>(), "number of server threads" )
			( "maxClients", prgOpts::value<unsigned short>(), "maximum number of clients" )
// network configuration
			( "listen", prgOpts::value< std::vector<std::string> >(), "addresses on which to listen (* for all)" )
			( "port", prgOpts::value<unsigned short>(), "TCP port" )
			( "SSLport", prgOpts::value<unsigned short>(), "TCP port for SSL connections" )

			( "idleTimeout", prgOpts::value<unsigned>(), "timeout for an idle connection" )
			( "requestTimeout", prgOpts::value<unsigned>(), "maximum time for a request" )
			( "answerTimeout", prgOpts::value<unsigned>(), "maximum time for an answer" )
// database configuration
			( "dbHost", prgOpts::value<std::string>(), "application database host" )
			( "dbPort", prgOpts::value<unsigned short>(), "application database port" )
			( "dbName", prgOpts::value<std::string>(), "application database name" )
			( "dbUser", prgOpts::value<std::string>(), "application database user" )
			( "dbPassword", prgOpts::value<std::string>(), "application database password" )
// logger configuration
			( "logFile", prgOpts::value<std::string>(), "log file" )
			;
	}

	bool CfgFileConfig::parse ( const char *filename )
	{
		file = resolvePath( boost::filesystem::system_complete( filename )).string();
		if ( !boost::filesystem::exists( file ))	{
			errMsg_ = "Configuration file ";
			errMsg_ += file;
			errMsg_ += " does not exist.";
			return false;
		}

		//

		return true;
	}

} // namespace _SMERP
