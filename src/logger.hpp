#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

class LogLevel	{
public:
	enum	{
		ALWAYS,		/// always log
		DATA,		/// log everything, including data
		TRACE,		/// trace functions calls
		DEBUG,		/// log operations
		INFO,
		NOTICE,
		WARNING,
		ERROR,
		SEVERE,
		CRITICAL,
		FATAL,
		NEVER		/// log this only under special conditions
	};
}

#endif // _LOGGER_HPP_INCLUDED
