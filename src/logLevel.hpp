//
// logLevel.hpp
//

#ifndef _LOG_LEVEL_HPP_INCLUDED
#define _LOG_LEVEL_HPP_INCLUDED

#include <string>

namespace _SMERP {

class LogLevel {
public:
	enum Level	{
		_SMERP_DATA,		/// log everything, including data
		_SMERP_TRACE,		/// trace functions calls
		_SMERP_DEBUG,		/// log operations
		_SMERP_INFO,
		_SMERP_NOTICE,
		_SMERP_WARNING,
		_SMERP_ERROR,
		_SMERP_SEVERE,
		_SMERP_CRITICAL,
		_SMERP_ALERT,
		_SMERP_FATAL,
		_SMERP_UNDEFINED	/// log this only under special conditions
	};

	static Level str2LogLevel( const std::string s );
};

} // namespace _SMERP


#endif // _LOG_LEVEL_HPP_INCLUDED
