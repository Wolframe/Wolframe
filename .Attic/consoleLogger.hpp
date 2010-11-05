//
// consoleLogger.hpp
//

#ifndef _CONSOLE_LOGGER_H_INCLUDED
#define _CONSOLE_LOGGER_H_INCLUDED

#include <string>
#include "logLevel.hpp"


namespace _dR {


class consoleLogger
{
public:
	consoleLogger( LogLevel::Level l = LogLevel::_dR_INFO );
	~consoleLogger();

	void level( LogLevel::Level l );
	LogLevel::Level level()			{ return level_; }

	void prefix( const std::string& prefixStr );
	const std::string& prefix()		{ return prefix_; }

	void log( LogLevel::Level l, const std::string& msg );

private:
	LogLevel::Level		level_;
	std::string		prefix_;
};


} // namespace _dR

#endif // _CONSOLE_LOGGER_H_INCLUDED
