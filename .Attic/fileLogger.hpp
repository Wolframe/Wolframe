//
// fileLogger.hpp
//

#ifndef _FILE_LOGGER_H_INCLUDED
#define _FILE_LOGGER_H_INCLUDED

#include <string>
#include <fstream>
#include "logLevel.hpp"


namespace _SMERP {


class fileLogger
{
public:
	fileLogger( std::string filename, LogLevel::Level l = LogLevel::_SMERP_INFO );
	~fileLogger( void );

	void level( LogLevel::Level l );
	LogLevel::Level level()				{ return level_; }

	void prefix( const std::string& prefix );
	const std::string& prefix()			{ return prefix_; }

	void reopen();
	void log( LogLevel::Level l, const std::string& msg );

private:
	std::ofstream		file_;
	LogLevel::Level		level_;
	std::string		filename_;
	std::string		prefix_;
};


} // namespace _SMERP

#endif // _FILE_LOGGER_H_INCLUDED
