#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

namespace _SMERP {

class LogLevel {
public:
	enum {
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

} // namespace _SMERP

#endif // _LOGGER_HPP_INCLUDED
