//
// fileLogger.cpp
//

#include <cassert>
#include <string>
#include <sstream>
#include <fstream>
#include "ErrorCode.hpp"
#include "appException.hpp"
#include "logLevel.hpp"
#include "fileLogger.hpp"


namespace _dR {


#include <sys/time.h>

inline std::string NowTime()
{
	char	buffer[64];
	time_t	t;

	time( &t );
	tm r = { 0 };
	strftime( buffer, sizeof( buffer ), "%b %e %X", localtime_r( &t, &r ));
/* no microseconds
	struct timeval tv;
	gettimeofday( &tv, 0 );
	char result[100] = { 0 };
	sprintf( result, "%s.%03ld ", buffer, (long)tv.tv_usec / 1000 );
	return result;
*/
	return buffer;
}


fileLogger::fileLogger( std::string filename, LogLevel::Level l )
{
	assert ( LogLevel::isLevel( l ) );
	level_ = l;
	filename_ = filename;
	prefix_.clear();
	file_.open( filename_.c_str(), std::ios_base::out | std::ios_base::app );
	if ( !file_ )
		throw systemException( ErrorCode::FILE_OPEN_ERROR, ErrorSeverity::SEVERITY_FATAL, ErrorModule::LOGGER,
				       "unable to open log file" );
}


fileLogger::~fileLogger()
{
	file_.close();
}


void fileLogger::level( LogLevel::Level l )
{
	assert ( LogLevel::isLevel( l ) );
	level_ = l;
}


void fileLogger::prefix( const std::string& prefixStr )
{
	prefix_ = " ";
	prefix_ += prefixStr;
	prefix_ += ": ";
}


void fileLogger::reopen()
{
	file_.close();
	file_.open( filename_.c_str(), std::ios_base::out | std::ios_base::app );
}


void fileLogger::log( LogLevel::Level l, const std::string& msg )
{
	if ( l >= level_ )	{
		std::ostringstream s;
		s << NowTime() << prefix_ << l << ": " << msg << std::endl;
		file_ << s.str();
		file_.flush();
	}
}

} // namespace _dR
