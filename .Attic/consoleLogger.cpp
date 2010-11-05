//
// consoleLogger.cpp
//

#include <cassert>
#include <string>
#include <sstream>
#include <iostream>
#include "logLevel.hpp"
#include "consoleLogger.hpp"


namespace _SMERP {

consoleLogger::consoleLogger( LogLevel::Level l )
{
	assert ( LogLevel::isLevel( l ) );
	level_ = l;
	prefix_.clear();
}


consoleLogger::~consoleLogger()
{
}


void consoleLogger::level( LogLevel::Level l )
{
	assert ( LogLevel::isLevel( l ) );
	level_ = l;
}


void consoleLogger::log( LogLevel::Level l, const std::string& msg )
{
	if ( l >= level_ )	{
		std::ostringstream s;
		s << prefix_ << l << ": " << msg << std::endl;
		std::cerr << s.str();
		std::cerr.flush();
	}
}


void consoleLogger::prefix( const std::string& prefixStr )
{
	prefix_ = prefixStr;
	prefix_ += ": ";
}


} // namespace _SMERP
