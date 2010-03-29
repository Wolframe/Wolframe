#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

namespace _SMERP {

class Logger {
public:
	enum LogLevel {
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
		ALERT,
		FATAL,
		NEVER		/// log this only under special conditions
	};

	static void initialize( );
};

} // namespace _SMERP

#endif // _LOGGER_HPP_INCLUDED
